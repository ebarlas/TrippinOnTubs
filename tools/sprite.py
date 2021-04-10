import xml.etree.ElementTree as ET
import subprocess
import json
import re
import math
from pathlib import Path
from PIL import Image

namespace = {
    'svg': 'http://www.w3.org/2000/svg'
}


def make_sprite_sheet(src_files, output_file):
    images = []
    for f in src_files:
        images.append(Image.open(f))

    image_width, image_height = images[0].size
    sheet_width = (image_width * len(images))
    sheet_height = image_height
    sheet = Image.new(
        mode='RGBA',
        size=(sheet_width, sheet_height),
        color=(0, 0, 0, 0))  # fully transparent
    for count, image in enumerate(images):
        x = image_width * count
        sheet.paste(image, (x, 0))
    sheet.save(output_file)


def fade(original, terminal, fade_to):
    d = terminal - original
    amount = d * fade_to
    return int(round(original + amount))


def fade_color(original, terminal, fade_to):
    red = fade(int(original[1:3], 16), terminal, fade_to)
    green = fade(int(original[3:5], 16), terminal, fade_to)
    blue = fade(int(original[5:7], 16), terminal, fade_to)
    return f'#{red:02x}{green:02x}{blue:02x}'


def fade_color_attr(attributes, key, terminal, fade_to):
    val = attributes.get(key)
    if val and val.startswith('#'):
        attributes[key] = fade_color(val, terminal, fade_to)


def replace_display(style, display):
    p = r'display:[a-zA-Z]+'
    return re.sub(p, display, style) if style and re.search(p, style) else display


def export_pngs(svg_file, tmp_dir, export_dir, scales, name):
    for d in (tmp_dir, export_dir):
        Path(d).mkdir(parents=True, exist_ok=True)

    tree = ET.parse(svg_file)
    root = tree.getroot()

    img_width = int(root.attrib['width'])
    img_height = int(root.attrib['height'])

    if 'scale' in root.attrib:
        scale = float(root.attrib['scale'])
        img_width *= scale
        img_height *= scale

    fade_to_white = float(root.attrib['fadeToWhite']) if 'fadeToWhite' in root.attrib else None

    num_frames = len(root.findall('.//svg:g[@type="frame"]', namespace))

    for n in range(1, num_frames + 1):
        if fade_to_white:
            for elem in ['path', 'circle', 'text']:
                for e in root.findall(f'.//svg:g[@type="frame"]/svg:{elem}', namespace):
                    styles = {s[0]: s[1] for s in [s.split(':') for s in e.attrib['style'].split(';')]}
                    fade_color_attr(styles, 'fill', 255, fade_to_white)
                    fade_color_attr(styles, 'stroke', 255, fade_to_white)
                    style = ';'.join([f'{k}:{styles[k]}' for k in styles.keys()])
                    e.set('style', style)
        for e in root.findall(f'.//svg:g[@type="frame"]', namespace):
            e.set('style', replace_display(e.get('style'), 'display:none'))
        for e in root.findall(f'.//svg:g[@type="frame"][@frame="{n}"]', namespace):
            e.set('style', replace_display(e.get('style'), 'display:inline'))
        tree.write(f'{tmp_dir}/{name}_tmp.svg')
        for scale in scales:
            subprocess.run([
                'inkscape',
                '--export-type=png',
                # ceil in lines below reflects preference to grow slightly rather than to shrink
                # prior to this change, touching ground tiles had small slivers of space between then at some scales
                f'--export-width={int(math.ceil(img_width * scale[1]))}',
                f'--export-height={int(math.ceil(img_height * scale[1]))}',
                f'{tmp_dir}/{name}_tmp.svg'])
            subprocess.run([
                'mv',
                f'{tmp_dir}/{name}_tmp.png',
                f'{export_dir}/{name}_{n}_{scale[0]}.png'])


def find_hit_box(svg_file):
    root = ET.parse(svg_file).getroot()

    scale = 1.0

    if 'scale' in root.attrib:
        scale = float(root.attrib['scale'])

    for e in root.findall('.//svg:g[@type="hitbox"]/svg:rect', namespace):
        return int(round(int(e.attrib['x']) * scale)), \
               int(round(int(e.attrib['y']) * scale)), \
               int(round(int(e.attrib['width']) * scale)), \
               int(round(int(e.attrib['height']) * scale))

    return 0, 0, int(root.attrib['width']) * scale, int(root.attrib['height']) * scale


def find_frame_duration(svg_file):
    root = ET.parse(svg_file).getroot()
    return int(root.attrib['duration']) if 'duration' in root.attrib else 80


def count_frames(svg_file, check_attrib=False):
    root = ET.parse(svg_file).getroot()
    if check_attrib and 'frames' in root.attrib:
        return int(root.attrib['scale'])
    return len(root.findall('.//svg:g[@type="frame"]', namespace))


def make_metadata(svg_file, output_file):
    Path(output_file).parent.mkdir(parents=True, exist_ok=True)
    num_frames = count_frames(svg_file, True)
    hit_box = find_hit_box(svg_file)
    duration = find_frame_duration(svg_file)
    metadata = {
        'frames': num_frames,
        'duration': duration,
        'hitBox': {
            'x': int(hit_box[0]),
            'y': int(hit_box[1]),
            'w': int(hit_box[2]),
            'h': int(hit_box[3])
        }
    }
    with open(output_file, 'w') as f:
        json.dump(metadata, f, indent=2)


def sanitize(svg_file, output_file):
    tree = ET.parse(svg_file)
    root = tree.getroot()
    for e in root.findall('svg:g[@type="development"]', namespace):
        root.remove(e)
    tree.write(output_file)

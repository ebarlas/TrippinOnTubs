import xml.etree.ElementTree as ET
import subprocess
import json
import re
import os
import shutil
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


def fade_style(e, fade_to_white):
    styles = {s[0]: s[1] for s in [s.split(':') for s in e.attrib['style'].split(';')]}
    fade_color_attr(styles, 'fill', 255, fade_to_white)
    fade_color_attr(styles, 'stroke', 255, fade_to_white)
    style = ';'.join([f'{k}:{styles[k]}' for k in styles.keys()])
    e.set('style', style)


def replace_display(style, display):
    p = r'display:[a-zA-Z]+'
    return re.sub(p, display, style) if style and re.search(p, style) else display


def export_pngs(svg_file, tmp_dir, export_dir, scales, name, unit_scale):
    tree = ET.parse(svg_file)
    root = tree.getroot()

    sprite_scale = unit_scale * (float(root.attrib['scale']) if 'scale' in root.attrib else 1.0)

    img_width = int(int(root.attrib['width']) * sprite_scale)
    img_height = int(int(root.attrib['height']) * sprite_scale)

    fade_to_white = float(root.attrib['fadeToWhite']) if 'fadeToWhite' in root.attrib else None

    num_frames = len(root.findall('.//svg:g[@type="frame"]', namespace))

    if fade_to_white:
        for elem in ['path', 'circle', 'text', 'rect']:
            for e in root.findall(f'.//svg:g[@type="frame"]/svg:{elem}', namespace):
                fade_style(e, fade_to_white)
            for e in root.findall(f'.//svg:g[@type="frame"]/svg:g/svg:{elem}', namespace):
                fade_style(e, fade_to_white)

    for scale in scales:
        dest_dir = f'{export_dir}/{name}/{scale[0]}'
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)

    for n in range(1, num_frames + 1):
        for e in root.findall(f'.//svg:g[@type="frame"]', namespace):
            e.set('style', replace_display(e.get('style'), 'display:none'))
        for e in root.findall(f'.//svg:g[@type="frame"][@frame="{n}"]', namespace):
            e.set('style', replace_display(e.get('style'), 'display:inline'))
        tree.write(f'{tmp_dir}/{name}_tmp.svg')
        for scale in scales:
            dst_png = f'{export_dir}/{name}/{scale[0]}/{name}_{n}.png'
            tmp_svg = f'{tmp_dir}/{name}_tmp.svg'
            tmp_png = f'{tmp_dir}/{name}_tmp.png'
            subprocess.run([
                'inkscape',
                '--export-type=png',
                f'--export-width={img_width * scale[1]}',
                f'--export-height={img_height * scale[1]}',
                tmp_svg])
            shutil.move(tmp_png, dst_png)
            print(dst_png)


def find_hit_box(svg_file, unit_scale):
    root = ET.parse(svg_file).getroot()

    sprite_scale = unit_scale * (float(root.attrib['scale']) if 'scale' in root.attrib else 1.0)

    for e in root.findall('.//svg:g[@type="hitbox"]/svg:rect', namespace):
        return int(e.attrib['x']) * sprite_scale, \
               int(e.attrib['y']) * sprite_scale, \
               int(e.attrib['width']) * sprite_scale, \
               int(e.attrib['height']) * sprite_scale

    return 0, 0, int(root.attrib['width']) * sprite_scale, int(root.attrib['height']) * sprite_scale


def find_frame_duration(svg_file):
    root = ET.parse(svg_file).getroot()
    return int(root.attrib['duration']) if 'duration' in root.attrib else 80


def count_frames(svg_file):
    root = ET.parse(svg_file).getroot()
    return len(root.findall('.//svg:g[@type="frame"]', namespace))


def make_metadata(svg_file, output_file, unit_scale):
    Path(output_file).parent.mkdir(parents=True, exist_ok=True)
    num_frames = count_frames(svg_file)
    hit_box = find_hit_box(svg_file, unit_scale)
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

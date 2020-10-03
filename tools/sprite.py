import xml.etree.ElementTree as ET
import subprocess
import json
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


def export_pngs(svg_file, tmp_dir, export_dir, scales, name):
    for d in (tmp_dir, export_dir):
        Path(d).mkdir(parents=True, exist_ok=True)

    tree = ET.parse(svg_file)
    root = tree.getroot()

    img_width = int(root.attrib['width'])
    img_height = int(root.attrib['height'])

    if 'scale' in root.attrib:
        scale = float(root.attrib['scale'])
        img_width = int(round(img_width * scale))
        img_height = int(round(img_height * scale))

    num_frames = len(root.findall('.//svg:g[@type="frame"]', namespace))

    for n in range(1, num_frames + 1):
        for e in root.findall(f'.//svg:g[@type="frame"]', namespace):
            e.set('style', 'display:none')
        for e in root.findall(f'.//svg:g[@type="frame"][@frame="{n}"]', namespace):
            e.set('style', 'display:inline')
        tree.write(f'{tmp_dir}/tmp.svg')
        for scale in scales:
            subprocess.run([
                'inkscape',
                '--export-type=png',
                f'--export-width={int(img_width * scale[1])}',
                f'--export-height={int(img_height * scale[1])}',
                f'{tmp_dir}/tmp.svg'])
            subprocess.run([
                'mv',
                f'{tmp_dir}/tmp.png',
                f'{export_dir}/{name}_{n}_{scale[0]}.png'])


def find_hit_box(svg_file):
    root = ET.parse(svg_file).getroot()

    scale = 1.0

    if 'scale' in root.attrib:
        scale = float(root.attrib['scale'])

    for e in root.findall('.//svg:g[@type="hitbox"]/svg:rect', namespace):
        return int(e.attrib['x']), \
               int(e.attrib['y']), \
               int(round(int(e.attrib['width']) * scale)), \
               int(round(int(e.attrib['height']) * scale))


def find_frame_duration(svg_file):
    root = ET.parse(svg_file).getroot()
    return int(root.attrib['duration'])


def count_frames(svg_file):
    root = ET.parse(svg_file).getroot()
    return len(root.findall('.//svg:g[@type="frame"]', namespace))


def make_metadata(svg_file, output_file):
    Path(output_file).parent.mkdir(parents=True, exist_ok=True)
    num_frames = count_frames(svg_file)
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

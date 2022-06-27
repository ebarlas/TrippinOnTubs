import xml.etree.ElementTree as ET
import json

namespace = {
    'svg': 'http://www.w3.org/2000/svg'
}


def extract_from_xml(node, name, target, fn):
    if name in node.attrib:
        target[name] = fn(node.attrib[name])


def find_objects(root):
    objects = []
    ints = ['lane', 'activation']
    bools = ['sparkle']
    id_counter = 1
    # an object is an image within a group that does not have attribute type='layer'
    for g in root.findall('.//svg:g', namespace):
        if g.attrib.get('type') not in ['layer', 'development']:
            for node in g.findall('.//svg:image', namespace):
                obj = {
                    'id': id_counter,
                    'type': node.attrib['type'],
                    'position': {
                        'x': int(node.attrib['x']),
                        'y': int(node.attrib['y'])
                    }
                }
                for s in ints:
                    extract_from_xml(node, s, obj, int)
                for s in bools:
                    extract_from_xml(node, s, obj, bool)
                if 'activateWhen' in node.attrib:
                    id = node.attrib['activateWhen']
                    ref = root.findall(f'.//*[@id="{id}"]')
                    if ref:
                        ref_x = int(ref[0].attrib['x'])
                        ref_activation = int(ref[0].attrib['activation'])
                        obj['activation'] = ref_activation + (obj['position']['x'] - ref_x)
                objects.append(obj)
                id_counter = id_counter + 1
    return objects


def find_layers(root):
    layers = []
    for g in root.findall('.//svg:g[@type="layer"]', namespace):
        layer = {
            'objects': []
        }
        if g.attrib.get('anchorTop') == 'true':
            layer['anchorTop'] = True
        layers.append(layer)
        for e in g.findall('.//svg:rect', namespace):
            layer['width'] = int(e.attrib['width'])
            layer['height'] = int(e.attrib['height'])
        for node in g.findall('.//svg:image', namespace):
            obj = {
                'type': node.attrib['type'],
                'position': {
                    'x': int(node.attrib['x']),
                    'y': int(node.attrib['y'])
                }
            }
            layer['objects'].append(obj)
    return layers


def make_model(svg_file):
    tree = ET.parse(svg_file)
    root = tree.getroot()
    return {
        'universe': {
            'width': int(root.attrib['width']),
            'height': int(root.attrib['height'])
        },
        'music': root.attrib['music'],
        'objects': find_objects(root),
        'layers': find_layers(root)
    }


def serialize_model(model, output_file):
    with open(output_file, 'w') as f:
        json.dump(model, f, indent=2)

import xml.etree.ElementTree as ET
import json

namespace = {
    'svg': 'http://www.w3.org/2000/svg'
}


def extract_float_from_xml(node, name, target):
    if name in node.attrib:
        target[name] = float(node.attrib[name])


def extract_int_from_xml(node, name, target):
    if name in node.attrib:
        target[name] = int(node.attrib[name])


def extract_float_point_from_xml(node, name, target):
    if f'{name}X' in node.attrib and f'{name}Y' in node.attrib:
        target[name] = {
            'x': float(node.attrib[f'{name}X']),
            'y': float(node.attrib[f'{name}Y']),
        }


def make_model(svg_file):
    tree = ET.parse(svg_file)
    root = tree.getroot()
    model = {
        'universe': {
            'width': int(root.attrib['width']),
            'height': int(root.attrib['height'])
        }
    }
    objects = []
    points = ['velocity', 'terminalVelocity', 'friction']
    floats = ['mass', 'gravity', 'fallGravity', 'minJumpVelocity', 'maxJumpVelocity',
              'runningAcceleration', 'risingAcceleration']
    ints = ['minJumpChargeTime', 'maxJumpChargeTime', 'jumpGracePeriod']
    id_counter = 1
    for node in root.findall('.//svg:image', namespace):
        obj = {
            'id': id_counter,
            'type': node.attrib['type'],
            'platform': node.attrib['platform'] == 'true',
            'position': {
                'x': int(node.attrib['x']),
                'y': int(node.attrib['y'])
            }
        }
        for p in points:
            extract_float_point_from_xml(node, p, obj)
        for s in floats:
            extract_float_from_xml(node, s, obj)
        for s in ints:
            extract_int_from_xml(node, s, obj)
        objects.append(obj)
        id_counter = id_counter + 1
    model['objects'] = objects
    return model


def serialize_model(model, output_file):
    with open(output_file, 'w') as f:
        json.dump(model, f, indent=2)

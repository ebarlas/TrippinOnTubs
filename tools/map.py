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


def extract_bool_from_xml(node, name, target):
    if name in node.attrib:
        target[name] = bool(node.attrib[name])


def extract_float_point_from_xml(node, name, target):
    if f'{name}X' in node.attrib and f'{name}Y' in node.attrib:
        target[name] = {
            'x': float(node.attrib[f'{name}X']),
            'y': float(node.attrib[f'{name}Y']),
        }


def find_objects(root):
    objects = []
    points = ['velocity', 'terminalVelocity', 'friction']
    floats = ['mass', 'massFactor', 'gravity', 'fallGravity', 'minJumpVelocity', 'maxJumpVelocity', 'maxDuckJumpVelocity',
              'runningAcceleration', 'risingAcceleration', 'duckFriction', 'coefficient']
    ints = ['minJumpChargeTime', 'maxJumpChargeTime', 'jumpGracePeriod', 'jumpSoundTimeout', 'lane', 'dustPeriod']
    bools = ['sparkle', 'accelerateWhenGrounded']
    id_counter = 1
    # an object is an image within a group that does not have attribute type='layer'
    for g in root.findall('.//svg:g', namespace):
        if g.attrib.get('type') not in ['layer', 'development']:
            for node in g.findall('.//svg:image', namespace):
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
                for s in bools:
                    extract_bool_from_xml(node, s, obj)
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
        'scale': float(root.attrib['scale']),
        'meterMargin': float(root.attrib['meterMargin']),
        'music': root.attrib['music'],
        'objects': find_objects(root),
        'layers': find_layers(root)
    }


def serialize_model(model, output_file):
    with open(output_file, 'w') as f:
        json.dump(model, f, indent=2)

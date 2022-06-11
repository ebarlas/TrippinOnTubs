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


def extract_int_from_xml_and_scale(node, name, target, scale):
    if name in node.attrib:
        target[name] = int(int(node.attrib[name]) * scale)


def extract_int_percentage_from_xml(node, name, target):
    if name in node.attrib:
        target[name] = int(float(node.attrib[name]) * 100)


def extract_bool_from_xml(node, name, target):
    if name in node.attrib:
        target[name] = bool(node.attrib[name])


def extract_int_point_from_xml(node, name, target, scale):
    if f'{name}X' in node.attrib and f'{name}Y' in node.attrib:
        target[name] = {
            'x': int(int(node.attrib[f'{name}X']) * scale),
            'y': int(int(node.attrib[f'{name}Y']) * scale),
        }


def find_objects(root, unit_scale):
    objects = []
    points = ['velocity', 'terminalVelocity', 'friction']
    ints = ['mass', 'minJumpChargeTime', 'maxJumpChargeTime', 'jumpGracePeriod', 'jumpSoundTimeout',
            'lane', 'dustPeriod', 'hitPoints']
    scaled_ints = ['gravity', 'fallGravity', 'minJumpVelocity', 'maxJumpVelocity', 'maxDuckJumpVelocity',
                   'runningAcceleration', 'risingAcceleration', 'duckFriction', 'activation']
    int_percentages = ['massFactor', 'coefficient']
    bools = ['sparkle', 'accelerateWhenGrounded', 'stompable', 'topStompable', 'bottomStompable',
             'randFrame', 'elasticObjectCollisions']
    id_counter = 1
    compat_scale = float(root.attrib['scale'])
    scale = unit_scale / compat_scale
    # an object is an image within a group that does not have attribute type='layer'
    for g in root.findall('.//svg:g', namespace):
        if g.attrib.get('type') not in ['layer', 'development']:
            for node in g.findall('.//svg:image', namespace):
                obj = {
                    'id': id_counter,
                    'type': node.attrib['type'],
                    'platform': node.attrib['platform'] == 'true',
                    'position': {
                        'x': int(int(node.attrib['x']) * scale),
                        'y': int(int(node.attrib['y']) * scale)
                    }
                }
                for p in points:
                    extract_int_point_from_xml(node, p, obj, scale)
                for s in ints:
                    extract_int_from_xml(node, s, obj)
                for s in scaled_ints:
                    extract_int_from_xml_and_scale(node, s, obj, scale)
                for s in int_percentages:
                    extract_int_percentage_from_xml(node, s, obj)
                for s in bools:
                    extract_bool_from_xml(node, s, obj)
                if 'activateWhen' in node.attrib:
                    id = node.attrib['activateWhen']
                    ref = root.findall(f'.//*[@id="{id}"]')
                    if ref:
                        ref_x = int(int(ref[0].attrib['x']) * scale)
                        ref_activation = int(int(ref[0].attrib['activation']) * scale)
                        obj['activation'] = ref_activation + (obj['position']['x'] - ref_x)
                objects.append(obj)
                id_counter = id_counter + 1
    return objects


def find_layers(root, unit_scale):
    compat_scale = float(root.attrib['scale'])
    scale = unit_scale / compat_scale
    layers = []
    for g in root.findall('.//svg:g[@type="layer"]', namespace):
        layer = {
            'objects': []
        }
        if g.attrib.get('anchorTop') == 'true':
            layer['anchorTop'] = True
        layers.append(layer)
        for e in g.findall('.//svg:rect', namespace):
            layer['width'] = int(int(e.attrib['width']) * scale)
            layer['height'] = int(int(e.attrib['height']) * scale)
        for node in g.findall('.//svg:image', namespace):
            obj = {
                'type': node.attrib['type'],
                'position': {
                    'x': int(int(node.attrib['x']) * scale),
                    'y': int(int(node.attrib['y']) * scale)
                }
            }
            extract_int_point_from_xml(node, 'velocity', obj, scale)
            extract_bool_from_xml(node, 'animated', obj)
            extract_bool_from_xml(node, 'randFrame', obj)
            layer['objects'].append(obj)
    return layers


def make_model(svg_file, unit_scale):
    tree = ET.parse(svg_file)
    root = tree.getroot()
    compat_scale = float(root.attrib['scale'])
    scale = unit_scale / compat_scale
    return {
        'universe': {
            'width': int(int(root.attrib['width']) * scale),
            'height': int(int(root.attrib['height']) * scale)
        },
        'meterMargin': int(int(root.attrib['meterMargin']) * scale),
        'music': root.attrib['music'],
        'objects': find_objects(root, unit_scale),
        'layers': find_layers(root, unit_scale)
    }


def serialize_model(model, output_file):
    with open(output_file, 'w') as f:
        json.dump(model, f, indent=2)

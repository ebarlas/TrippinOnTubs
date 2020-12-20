import sprite
import map
import sys

scales = [
    ('xxxsmall', 0.25),
    ('xxsmall', 0.375),
    ('xsmall', 0.5),
    ('small', 0.75),
    ('medium', 1)
]

sprites = [
    'goggin',
    'ball',
    'ground',
    'platformmid',
    'platformleft',
    'platformright',
    'ground_melt_left',
    'ground_melt_middle',
    'ground_melt_right',
    'winged_tub',
    'clock_timer'
]

levels = ['map', 'map_ground_melt']

dir_raw = 'raw'
dir_svgs = 'svgs'
dir_build = 'build-sprites'
dir_sprites = 'sprites'
dir_maps = 'maps'


def sprite_assets(proj_home):
    raw_dir = f'{proj_home}/{dir_raw}'
    src_dir = f'{proj_home}/{dir_svgs}'
    build_dir = f'{proj_home}/{dir_build}'
    sprites_dir = f'{proj_home}/{dir_sprites}'

    for spr in sprites:
        raw_file = f'{raw_dir}/{spr}.svg'
        svg_file = f'{src_dir}/{spr}.svg'
        meta_file = f'{sprites_dir}/{spr}/{spr}.json'

        sprite.sanitize(raw_file, svg_file)

        num_frames = sprite.count_frames(svg_file)
        sprite.make_metadata(svg_file, meta_file)
        sprite.export_pngs(svg_file, build_dir, build_dir, scales, spr)
        for scale in scales:
            src_files = [f'{build_dir}/{spr}_{n + 1}_{scale[0]}.png' for n in range(num_frames)]
            output_file = f'{sprites_dir}/{spr}/{spr}_{scale[0]}.png'
            sprite.make_sprite_sheet(src_files, output_file)


def level_assets(proj_home):
    src_dir = f'{proj_home}/{dir_raw}'
    map_dir = f'{proj_home}/{dir_maps}'

    for level in levels:
        svg_file = f'{src_dir}/{level}.svg'
        map_json = f'{map_dir}/{level}.json'
        model = map.make_model(svg_file)
        map.serialize_model(model, map_json)


def main():
    proj_home = '..'
    if len(sys.argv) > 1:
        proj_home = sys.argv[1]

    sprite_assets(proj_home)
    level_assets(proj_home)


if __name__ == '__main__':
    main()

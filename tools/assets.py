import sprite
import map
import sys
import concurrent.futures

scales = [
    ('map', 0.25), ('svga', 0.09375), ('sxga', 0.15), ('hdplus', 0.1875), ('fhd', 0.225), ('qhd', 0.3), ('qhdplus', 0.346875), ('uhd', 0.45)
]

sprites = [
    'goggin',
    'ball',
    'ground',
    'ground_melt_left',
    'ground_melt_middle',
    'ground_melt_right',
    'winged_tub',
    'clock_timer',
    'winged_foot',
    'digits',
    'alpha',
    'zombie',
    'rat',
    'phoenix',
    'masonic_lodge',
    'hills',
    'hills_base',
    'hotel_petaluma',
    'dust',
    'dust_blast',
    'running_clock',
    'bird',
    'trippin',
    'start',
    'high_score',
    'save_score',
    'exit',
    'all_time',
    'today',
    'street_light',
    'cloud',
    'brick',
    'level',
    'walking_tub'
]

levels = ['map_ground_melt', 'map_minimal', 'map_test', 'map_load', 'map_training']

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

    def process_sprite(spr):
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

    with concurrent.futures.ThreadPoolExecutor(max_workers=15) as executor:
        for spr in sprites:
            executor.submit(process_sprite, spr)


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

import sprite
import map
import sys
import concurrent.futures

# Roughly corresponds to device min-width of 600 px
# The small floating value reflects the fact that the dimensions of raw SVGs are hugely inflated
# Raw SVG scale is high to ease drafting, with grid lines on 1-px or 10-px boundaries
unit_scale = 0.0703125 / 2

scales = [
    ('1x', 1), ('2x', 2), ('3x', 3), ('4x', 4), ('5x', 5), ('6x', 6), ('7x', 7), ('8x', 8), ('9x', 9), ('10x', 10)
]

sprites = [
    'goggin',
    'ball',
    'fireball',
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
    'hills_dark',
    'hills_base_dark',
    'cloud_dark',
    'hotel_petaluma',
    'dust',
    'dust_blast',
    'dust_blast_white',
    'running_clock',
    'bird',
    'trippin',
    'start',
    'train',
    'jump',
    'duck',
    'charged_jump',
    'charged_duck_jump',
    'double_jump',
    'high_score',
    'high_score_title',
    'save_score',
    'exit',
    'all_time',
    'today',
    'street_light',
    'cloud',
    'brick',
    'level',
    'walking_tub',
    'walking_tub_green',
    'walking_tub_yellow',
    'mystic_theatre',
    'goggin_head',
    'combo',
    'stop',
    'jump_slam_down',
    'duck_controls',
    'jump_controls',
    'double_jump_controls',
    'stop_controls',
    'charged_jump_controls',
    'charged_duck_jump_controls',
    'jump_slam_down_controls',
    'chicken',
    'sparkle',
    'flame',
    'group_bonus',
    'petaluma_museum',
    'time_bonus',
    'gameover',
    'levels_completed',
    'generic_building',
    'generic_building_2',
    'level_time_bonus',
    'level_completed_bonus',
    'extra_lives_bonus',
    'game_completed_bonus',
    'replay_speed_up',
    'training_completed',
    'training_status',
    'tap_score_exit',
    'double_jump_arc',
    'jump_arc',
    'charged_jump_arc',
    'duck_line',
    'jump_slam_down_arc',
    'charged_duck_jump_arc',
    'tap_flags_exit',
    'top_scores',
    'latest_scores',
    'my_scores'
    'zombie_green',
    'post_office',
]

levels = [
    'level_1',
    'level_2',
    'level_3',
    'level_4',
    'level_5',
    'level_6',
    'level_7',
    'level_8',
    'load',
    'training_jump',
    'training_duck',
    'training_double_jump',
    'training_charged_jump',
    'training_charged_duck_jump',
    'training_jump_slam_down'
]

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

        sprite.make_metadata(svg_file, meta_file, unit_scale)
        sprite.export_pngs(svg_file, build_dir, sprites_dir, scales, spr, unit_scale)

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

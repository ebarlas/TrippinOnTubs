import gif
import sprite
import map


def gif_test():
    gif_file = 'sample.gif'
    output_dir = 'frames'
    name = 'sample'
    gif.print_gif(gif_file)
    gif.extract_frames(gif_file, output_dir, name)


def sprite_test():
    raw_file = 'sample.svg'
    svg_file = 'sanitized.svg'
    sprite.sanitize(raw_file, svg_file)
    print(sprite.find_hit_box(svg_file))
    print(sprite.find_frame_duration(svg_file))
    sprite.export_pngs(svg_file, 'tmp', 'exports', [('small', 0.5), ('medium', 1.0)], 'sample')
    sprite.make_metadata(svg_file, 'tmp/metadata.json')


def map_test():
    src_file = '../raw/map.svg'
    dst_file = 'map.json'
    model = map.make_model(src_file)
    map.serialize_model(model, dst_file)


def animate_frames():
    # gif.animate_frames('../build-sprites/winged_foot_', '_medium.png', 11, 'winged_foot.gif', 80)
    gif.animate_frames('../build-sprites/clock_timer_', '_medium.png', 11, 'clock_timer.gif', 80)


if __name__ == '__main__':
    # gif_test()
    # sprite_test()
    # map_test()
    animate_frames()

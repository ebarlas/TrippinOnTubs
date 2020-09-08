import gif
import sprite


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


if __name__ == '__main__':
    gif_test()
    sprite_test()
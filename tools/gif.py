from PIL import Image
from pathlib import Path


def print_gif(gif_file):
    im = Image.open(gif_file)

    print(f'animated: {im.is_animated}')
    print(f'frames: {im.n_frames}')

    for n in range(0, im.n_frames):
        im.seek(n)
        print(f'frame[{n}]: {im.info}')


def extract_frames(gif_file, output_dir, name):
    Path(output_dir).mkdir(parents=True, exist_ok=True)

    im = Image.open(gif_file)

    digits = len(str(im.n_frames))

    for n in range(0, im.n_frames):
        im.seek(n)
        frame = im.copy()
        padded = format(n, f'0{digits}')
        frame.save(f'{output_dir}/{name}{padded}.gif')


def cut_frames(gif_file, output_file, frame_step, duration):
    im = Image.open(gif_file)

    images = []
    for n in range(0, im.n_frames, frame_step):
        im.seek(n)
        frame = im.copy()
        images.append(frame)

    images[0].save(output_file,
                   save_all=True,
                   append_images=images[1:],
                   optimize=False,
                   duration=duration,
                   loop=0,
                   transparency=0)


def animate_frames(file_prefix, file_suffix, count, gif_file, duration=80):
    images = []
    for n in range(1, count + 1):
        images.append(Image.open(f'{file_prefix}{12-n}{file_suffix}'))

    images[0].save(gif_file, save_all=True, append_images=images[1:], optimize=False, duration=duration, loop=0)

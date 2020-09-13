import sys
from PIL import Image

def test_color(img_file, x, y):
    im = Image.open(img_file)
    rgb = im.convert('RGB')
    r, g, b = rgb.getpixel((x, y))
    print(f'rgb({x}, {y}) = ({r}, {g}, {b})')


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print('3 arguments required: [image file] [x] [y]')
        sys.exit(1)

    test_color(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))
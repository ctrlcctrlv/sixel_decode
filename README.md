# `sixel_decode`

This utility dumps the palette of a file that is sixel encoded and outputs an RGB bitmap with no header.

If output specified, you'll get width and height on stdout separated by <kbd>x</kbd>, like `32x32`.

If you have ImageMagick or GraphicsMagick, you could get a PNG out like this:

```bash
convert -size 971x747 -depth 8 /tmp/abbr2.rgb /tmp/abbr2.png
```

## Compiling

Run `make`. Obviously you need [`libsixel`](https://github.com/saitoha/libsixel).

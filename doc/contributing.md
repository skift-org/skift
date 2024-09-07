# Contributing

Contributions are welcome and encouraged for this project! We practice [optimistic merging](http://hintjens.com/blog:106), which means that pull requests are merged into the main branch as soon as possible. The goal is to keep pull requests small, focused, and incremental.

When writing commit messages, please keep them short and concise. They should be prefixed with the name of the package that the commit affects. For example:

```
karm-base: Fix buffer overflow in Karm::String::reserve.
```

Binary files should be kept as small as possible. Here are some tips for reducing the size of different image formats:
- Use SVG instead of other raster image formats whenever possible.
- Use `optipng -strip all` to reduce the size of PNG images.
- Use `gifsicle -O3` to reduce the size of GIF images.
- Use `jpegoptim -m90` to reduce the size of JPEG images.

## Coding Style

Just configure you IDE to use the `.clang-format` file at the root of the source tree, or use `meta/scripts/style-format.sh` to format the code.

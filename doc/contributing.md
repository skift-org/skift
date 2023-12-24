<br>
<img src="logo-light.svg#gh-light-mode-only" height="24" />
<img src="logo-dark.svg#gh-dark-mode-only" height="24" />
<br>
<br>

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

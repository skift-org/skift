<br/>
<p align="center">
  <a  href="https://github.com/skiftOS/skift/tree/legacy">Looking for the old skiftOS? Click here</a>
</p>
<br/>

![](doc/header.svg)

<br/>
<p align="center">
  <a href="https://skiftos.org/">Website</a> -
  <a href="doc/readme.md">Manual</a> -
  <a href="http://discord.skiftos.org">Discord</a> -
  <a href="https://github.com/skiftOS/skift/releases">Releases</a>
</p>
<br/>

# **skiftOS**: The delightful operating system

**skiftOS** is a hobby operating system created from scratch in contemporary C++ for ARM, x86, and RISC-V platforms. We built Skift on top of the _Hjert_ micro-kernel, a C++ rewrite of the _BRUTAL_ kernel to make it simpler, safer, and enhance the existing design. On top of the kernel resides _Karm_, a framework comprised of a minimal subset of the C and C++ standard library, an extensive collection of containers and utilities, a SwiftUI inspired Ui, and much more. Finally, built on top of _Karm_ is _Hideo_, a delightful desktop environment.

## Building

skiftOS is written in bleeding-edge C2x and C++23 and building it requires a modern C/C++ compiler like clang-14 or GCC-12 installed on the host machine.

## Contributings

This project practice [optimistic merging](http://hintjens.com/blog:106) meaning that pull requests are merged into the main branch as soon as possible. The objective is to keep PR as small, focused, and incremental as possible.

Commit messages should be short and concise and prefixed with the name of the package. For example:

```
karm-base: Fix buffer overflow in Karm::String::reserve.
```

Binary files should be should be as small as possible. Use:

- SVG should be preferred over other raster images formats
- `optipng -strip all` to reduce the size of PNG images.
- `gifsicle -O3` to reduce the size of GIF images.
- `jpegoptim -m90` to reduce the size of JPEG images.

## License

<a href="https://opensource.org/licenses/MIT">
  <img align="right" height="96" alt="MIT License" src="doc/mit.svg" />
</a>

The skift operating system and its core components are licensed under the **MIT License**.

The full text of the license can be accessed via [this link](https://opensource.org/licenses/MIT) and is also included in the [license.md](license.md) file of this software package.

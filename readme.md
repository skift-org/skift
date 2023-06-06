<br/>
<br/>
<br/>

<p align="center">
  <img src="doc/logo-light.svg#gh-light-mode-only" height="64" />
  <img src="doc/logo-dark.svg#gh-dark-mode-only" height="64" />
</p>

<br/>

<p align="center">
  <a href="https://skiftos.org/">Website</a> -
  <a href="http://discord.skiftos.org">Discord</a> -
  <a href="https://github.com/skiftOS/skift/releases">Releases</a>
</p>

<br/>
<br/>
<br/>


## About

**skiftOS** is a hobby operating system built from scratch using contemporary C and C++ for ARM, x86, and RISC-V architectures. It is designed to be simple, modern, and modular. skiftOS features a modern C++ core library, modern reactive UI, and a capability-based microkernel for security and modularity.

<p align="center">
<img src="doc/screenshots/2023-06-06.png" />
<br>
skiftOS applications running on Linux
</p>

## Values

As a hobby operating system, skiftOS is built with the following values in mind:

- **Simple**: skiftOS aims to be simple and easy to use, without being overly complex or difficult to understand. It prioritizes user-friendliness over feature bloat.

- **Modern**: skiftOS is built using modern C++ and C standards, and is designed to be extensible and hackable. It leverages modern programming techniques and technologies to provide a robust and efficient operating system.

- **Modular**: skiftOS is designed to be modular and extensible. It is built around a microkernel architecture, which allows for the addition of new features without having to modify the core components of the operating system.

## Features

skiftOS has several notable features:

- **Modern C++ Core Library**: skiftOS is built using karm, a modern C++ core library that provides essential functionality like memory management, file I/O, and networking. karm takes inspiration from [The Rust Standard Library](https://doc.rust-lang.org/std/) to create a powerful and safe library.

- **Modern Reactive UI**: skiftOS uses karm-ui, a modern reactive UI library that is based on unidirectional data flow. It takes inspiration from popular UI frameworks like React and Flutter, and enables developers to create powerful and responsive user interfaces.

- **Capability-based Microkernel**: skiftOS is built around a microkernel architecture that uses capability-based security. This means that each process has only the permissions it needs to perform its tasks, which makes the operating system more secure and easier to maintain.

- **Multi-architecture Support**: skiftOS can run on multiple architectures, including ARM, x86, and RISC-V. This makes it highly versatile and allows it to run on a variety of devices.

- **Easy to Hack On**: skiftOS is designed to be easy to hack on, with a focus on simplicity and modularity. This means that developers can easily contribute to the project and add new features without having to understand the entire codebase.

- **Free and Open-Source**: skiftOS is licensed under the MIT license and is free to use, modify, and redistribute. This makes it accessible to everyone and encourages collaboration and innovation.


## Building

skiftOS is written in bleeding-edge C23 and C++23 and building it requires a modern C/C++ compiler like clang-14 or GCC-12 installed on the host machine.

```sh
# Make sure clang is the right version
$ clang --version
clang version 13.0.1
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin

# Make sure nasm is installed
$ nasm --version
NASM version 2.15.05 compiled on Sep 24 2020

# Make sure python3 is installed
$ python3 --version
Python 3.10.5
```

Building skiftOS also requires installing [CuteKit](https://github.com/cute-engineering/cutekit)

> CuteKit is a package manager and build system for C/C++/ASM/RUST projects. It is designed to meet the needs of hobby operating systems and other low-level projects.

```sh
$ git clone https://github.com/cute-engineering/cutekit

$ cd cutekit

$ git switch dev

$ pip install --user -e .
```

Once you have installed CuteKit, you can install the dependencies and build the operating system:

```sh
$ cutekit install

$ cutekit build --all
```

Finally, you can run the operating system using:

```sh
$ cutekit start
```

> After pulling new changes, you can run `cutekit clean` to remove the build artifacts and `cutekit build` to rebuild the operating system.

Individual components can be run on the host system using:

```sh
$ cutekit run <component>
```

> Use `cutekit help` to get more information about the available commands.

## Contributing

Contributions are welcome and encouraged!

This project practice [optimistic merging](http://hintjens.com/blog:106) meaning that pull requests are merged into the main branch as soon as possible. The objective is to keep PR as small, focused, and incremental as possible.

Commit messages should be short and concise and prefixed with the name of the package. For example:

```

karm-base: Fix buffer overflow in Karm::String::reserve.

```

Binary files should be as small as possible.

- SVG should be preferred over other raster images formats
- `optipng -strip all` to reduce the size of PNG images.
- `gifsicle -O3` to reduce the size of GIF images.
- `jpegoptim -m90` to reduce the size of JPEG images.

## Acknowledgements

I (sleepy-monax) would like to thank the following people for their help and support:
 - [Cyp](https://cyp.sh), [Keyboard Slayer](https://github.com/keyboard-slayer), and [D0p1](https://github.com/d0p1s4m4) for being great friends giving me a lot of support and motivation.
 - [Feliwir](https://github.com/feliwir) for all the work he did on the past skiftOS and BRUTAL project. I learned a lot from his work and I am very grateful for his help.
 - And all the people who have contributed to the project and send me money on [Github Sponsors](https://github.com/sponsors/sleepy-monax)

## License

<a href="https://opensource.org/licenses/MIT">
  <img align="right" height="96" alt="MIT License" src="doc/mit.svg" />
</a>

The skift operating system and its core components are licensed under the **MIT License**.

The full text of the license can be accessed via [this link](https://opensource.org/licenses/MIT) and is also included in the [license.md](license.md) file of this software package.

<br/>
<br/>
<br/>
<br/>
<br/>

<p align="center">
  <img src="doc/logo-light.svg#gh-light-mode-only" height="96" />
  <img src="doc/logo-dark.svg#gh-dark-mode-only" height="96" />
</p>

<p align="center">
  <a href="https://skiftos.org/">Website</a> -
  <a href="http://discord.skiftos.org">Discord</a> -
  <a href="https://github.com/skiftOS/skift/releases">Releases</a>
</p>

<br/>
<br/>
<br/>
<br/>
<br/>

# **skiftOS**: The delightful operating system

[![Build](https://github.com/skift-org/skift/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/skift-org/skift/actions/workflows/build.yml)
[![License](https://img.shields.io/github/license/skift-org/skift)](https://opensource.org/licenses/MIT)
[![Discord](https://img.shields.io/discord/555284858180730888?label=discord)](http://discord.skiftos.org)

# Table of Contents

- [Table of Contents](#table-of-contents)
- [Introduction](#introduction)
  - [Values](#values)
  - [Features](#features)
  - [Screenshot](#screenshot)
- [Getting Started](#getting-started)
  - [Building](#building)
  - [Contributing](#contributing)
- [Acknowledgements](#acknowledgements)
- [License](#license)

# Introduction

**skiftOS** is a hobby operating system built from scratch in contemporary C and C++ for ARM, x86, and RISC-V architectures.

Following the 80/20 rule, **skift** tries to be a simple, yet powerful operating system that can be used for daily tasks. It is designed to be modular, extensible, and easy to use and hack on.

## Values

As a hobby operating system, skiftOS is built with the following values in mind:

- **Simple**: skiftOS is designed to be simple and easy to use. It is not meant to be a full-featured operating system, but rather a simple, yet powerful operating system that can be used for daily tasks.
- **Modern**: skiftOS is built using modern C++ and C standards and is designed to be extensible and hackable.
- **Modular**: skiftOS is designed to be modular and extensible. It is possible to add new features to the operating system without having to modify the core components.
- **Open**: skiftOS is open-source and licensed under the MIT license. It is free to use, modify, and redistribute.
- **Inclusive**: skiftOS is built by a diverse group of people from all around the world. It is designed to be inclusive and welcoming to everyone.

## Features

- **Modern C++ Core Library**: skiftOS is built using [karm](src/libs/) a modern C++ core library build by taking the best of the Rust and C++ worlds.
- **Modern Reactive UI**: Applications are built using [karm-ui](src/libs/karm-ui) a modern reactive UI library build around unidirectional data flow inspired by [React](https://reactjs.org/) and [Flutter](https://flutter.dev/).
- **Capability-based Microkernel**: skiftOS is built around a capability-based microkernel that provides a secure and modular environment for applications.

## Screenshot

<p align="center">
<img src="doc/screenshots/2022-09-07.png" />
<br>
skiftOS applications running on Linux
</p>

# Getting Started

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

Building skiftOS also requires installing [osdk](https://github.com/devse-org/osdk):

> OSDK is a package manager and build system for C/C++/ASM/RUST projects. It is designed to meet the needs of hobby operating systems and other low-level projects.

```sh
$ git clone https://github.com/devse-org/osdk

$ cd osdk

$ pip install --user -e .
```

Once you have installed osdk, you can install the dependencies and build the operating system:

```sh
$ osdk install

$ osdk build --all
```

Finally, you can run the operating system using:

```sh
$ osdk start
```

> After pulling new changes, you can run `osdk clean` to remove the build artifacts and `osdk build --all` to rebuild the operating system.

Individual components can be run on the host system using:

```sh
$ osdk run <component>
```

> Use `osdk help` to get more information about the available commands.

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

# Acknowledgements

I (sleepy-monax) would like to thank the following people for their help and support:
 - [Cyp](https://cyp.sh), [Keyboard Slayer](https://github.com/keyboard-slayer), and [D0p1](https://github.com/d0p1s4m4) for being great friends giving me a lot of support and motivation.
 - [Feliwir](https://github.com/feliwir) for all the work he did on the past skiftOS and BRUTAL project. I learned a lot from his work and I am very grateful for his help.
 - And all the people who have contributed to the project and send me money on [Github Sponsors](https://github.com/sponsors/sleepy-monax)

## Contributors

<a href="https://github.com/skiftOS/skift/graphs/contributors">
  <img src="https://contributors-img.web.app/image?repo=skiftOS/skift" />
</a>

## Projects

- [The OSDev Wiki](https://wiki.osdev.org/Main_Page)
- [The DEVSE Community](https://devse.wiki)
- [The Limine Project](https://limine-bootloader.org/)
- [The BRUTAL Project](https://brutal.smnx.sh)
- [The SerenityOS Project](https://serenityos.org/)

# License

<a href="https://opensource.org/licenses/MIT">
  <img align="right" height="96" alt="MIT License" src="doc/mit.svg" />
</a>

The skift operating system and its core components are licensed under the **MIT License**.

The full text of the license can be accessed via [this link](https://opensource.org/licenses/MIT) and is also included in the [license.md](license.md) file of this software package.

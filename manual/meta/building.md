# Build Guide

## Table of content

- [Build Guide](#build-guide)
  - [Table of content](#table-of-content)
  - [Supported environment](#supported-environment)
    - [About WSL](#about-wsl)
  - [Building skiftOS](#building-skiftos)
    - [1. Get the source code](#1-get-the-source-code)
    - [2. Setting up](#2-setting-up)
    - [3. Building](#3-building)
    - [4. Running in a virtual machine](#4-running-in-a-virtual-machine)
    - [5. Tips](#5-tips)
    - [6. Using the system](#6-using-the-system)
    - [7. Contributing](#7-contributing)

## Supported environment

Building skiftOS requires

- A good Linux distribution
- nasm
- gcc
- binutils
- grub
- ImageMagick

And for testing and debugging
- qemu
- gdb

```sh
# On Debian or Debian-based distributions
$ sudo apt install nasm gcc make binutils grub-pc-bin xorriso mtools git qemu qemu-kvm
```

```sh
# On Arch or Arch-based distributions
$ sudo pacman -S nasm gcc make binutils grub qemu libisoburn mtools git qemu
```

```sh
# On Red Hat or Red Hat-based distributions
$ sudo dnf install nasm gcc gcc-g++ make binutils xorriso mtools ImageMagick git qemu qemu-kvm patch
```

### About WSL

It's possible to build skiftOS WSL1 and WSL2 but it's not well tested.
If you have any problems consider upgrading to a GNU/linux distribution.

## Building skiftOS

### 1. Get the source code

Clone the repository with all its submodules.

```sh
$ git clone --recursive https://github.com/skiftOS/skift

$ cd skift
```

Or if you have already cloned this repo without `--recursive` do:

```sh
$ cd skift

$ git submodule init
```

### 2. Setting up

Building the toolchain is pretty straight-forward,
first make sure you have all GCC and binutils dependencies:
 - build-essential
 - bison
 - flex
 - libgmp3-dev
 - libmpc-dev
 - libmpfr-dev
 - texinfo

You can run the following command on ubuntu:

```sh
# On Debian or Debian-based distributions
$ sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
```

```sh
# On Arch or Arch-based distributions
$ sudo pacman -S base-devel bison flex mpc mpfr texinfo
```

```sh
# On Red Hat or Red Hat-based distributions
$ sudo dnf install bison flex mpc-devel mpfr-devel gmp-devel texinfo patch 
```

Then for building the toolchain run the `build-it.sh` script

```sh
## Build the tool chain
$ toolchain/build-it.sh

## Then wait for completion
```

The script will do the following operation without installing anything to the host system nor requiering root access:
 - Download `gcc` and `binutils` from the GNU project
 - Patch them using binutils.patch and gcc.patch which are located in the toolchain directory.
 - Then configure and build

### 3. Building

From the root of this repo do:

```sh
$ make all
```

This command will build all the components of the operating system and generate an ISO bootable in QEMU or VirtualBox.

> The compatibility with virtual box is not guaranteed, as we use QEMU primarly for debugging and testing the system.

### 4. Running in a virtual machine

The build system allows you to create and start a virtual machine of skiftOS by using one of the following commands:

```sh
$ make run CONFIG_VMACHINE=qemu # for QEMU
# or
$ make run CONFIG_VMACHINE=vbox # for Virtual Box
```

### 5. Tips

> If you made any modification to the source code or the content of the sysroot/ directory, the build system should be able to rebuild the project from step 3 automagically :^)

> You can change the default keyboard layout by passing CONFIG_KEYBOARD_LAYOUT="fr_fr" to make.

### 6. Using the system

**How to change the keyboard layout?**

```sh
µ keyboardctl en_us
```

**How to change display resolution?**

```sh
µ displayctl -s 1920x1080
```
**How to change to wallpaper?**

```sh
µ wallpaperctl /Files/Wallpapers/paint.png
```

### 7. Contributing

A bug? A Missing feature? Please consider contributing to the project :hugs: ❤️

See [contributing.md](contributing.md)

# Building

## 0. Supported environment

Building skiftOS requires

- A good linux distro
- nasm
- gcc
- binutils
- grub

And for testing et debuging
- qemu-system-i386
- gdb


```sh
# On Ubuntu
$ apt install nasm gcc make binutils grub-pc-bin qemu-system-x86 xorriso mtools
```

## 1. Setting up

Building the toolchain is pretty straight-forward,
first make sure you have all GCC and binutils dependancies:
 - build-essential
 - bison
 - flex
 - libgmp3-dev
 - libmpc-dev
 - libmpfr-dev
 - texinfo

You can run the following command on ubuntu:
```sh
$ apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
```

Then for building the toolchain run the `build-it!.sh` script

```sh
## Build the tool chain
$ toolchain/build-it!.sh

## Then wait for completion
```

The script will do the following operation without installing anything to the host system nor requiering root access:
 - Download `gcc` and `binutils` from the GNU project
 - Patch them using binutils.patch and gcc.patch located in the toolchain directory.
 - Then configure and build

## 2. Building

From the root of this repo do:

```sh
make all
```

This command will build all the component of the project and generate an ISO bootable on QEMU or VirtualBox

> The compatibility with virtual box is not guaranteed, as we use QEMU primarly for debuging and testing the system

## 3. Running in qemu

From the root of this repo do:

```sh
make run
```

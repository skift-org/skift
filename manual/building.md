# Building

## Supported environment

Building skiftOS required

- A good linux distro
- nasm
- gcc
- binutils
- python 3.7
- grub

And for testing et debuging
- qemu-system-i386
- gdb
- suckless terminal (if you whant to use `./automagicaly debug`)


```sh
# On Ubuntu
apt install nasm gcc make binutils python3 grub-pc-bin qemu-system-x86
```

## Setting up the toolchain

Building the toolchain is pretty straight-forward.
First make sure you have all GCC and binutils dependancies:

- build-essential
- bison
- flex
- libgmp3-dev
- libmpc-dev
- libmpfr-dev
- texinfo

Install the dependancies on ubuntu:
```sh
apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
```

Then from the root of this repo do:

```sh
## Build the tool chain
toolchain/build-it!.sh

## Then wait for completion
```

### More info
  - [Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) - OSDEV.org

## Building skiftOS
From the root of this repo do:

```sh
# For a simple build
./automagicaly build-all

# For a clean build
./automagicaly rebuild-all

## Then wait for completion
```

## Testing

From the root of this repo do:

```sh
# Run the operation system in QEMU
./automagicaly run

# Run the operating system in QEMU with debugging (WIP)
# Required gdb and the suckless terminal emulator.
./automagicaly debug skift.hjert.kernel
```
# Building

## Supported environement

Building skiftOS required

- Ubuntu 18.04 or 16.04
- nasm >= 2.13
- gcc 7.3
- binutils
- python 3.7
- grub-pc-bin

For testing
- qemu-system-i386

```sh
# On Ubuntu
apt install nasm gcc make binutils python3 grub-pc-bin qemu-system-x86
```

## Setting up the toolchain

Building the toolchain is pretty straightforward.
First make sure you have all gcc and binutils dependancies:

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
## Change the current directory
cd toolchain/

## Build the tool chain
./build-it!.sh

## Then wait for completion
```

### More info
  - [CC_Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) - OSDEV.org

## Building skiftOS
From the root of this repo do:

```sh
# For a simple build
./automagicaly build-all

# For a clean build
./automagicaly rebuild-all
```

## Testing

From the root of this repo do:

```sh
# Run the operation system in qemu
./automagicaly run

# Run the operating system in qemu with debugging (WIP)
./automagicaly debug
```
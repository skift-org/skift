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

Building the toolchain is pretty strainforward.
First make sure you have all gcc and binutils dependancies:

- build-essential
- bison
- flex
- libgmp3-dev
- libmpc-dev
- libmpfr-dev
- texinfo

```sh
# On Ubuntu
apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
```

Then from the root of this repo do:

```sh
## Change the current directory
cd toolchain/

## Build the tool chain
./build-it!.sh

## Then wait for complition
```

## Building skiftOS
From the root of this repo do:

```sh
# For a simple build
./SOSBS.py build-all

# For a clean build (release)
./SOSBS.py rebuild-all
```

## Testing

From the root of this repo do:

```sh
# Run the operation system in qemu
./SOSBS.py run

# Run the operating system in qemu with debugging (WIP)
./SOSBS.py debug
```
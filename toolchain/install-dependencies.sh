#!/bin/bash
apt update
apt install -y nasm gcc make binutils grub-pc-bin qemu-system-x86
apt install -y build-essential bison flex libgmp3-dev libmpc-dev
apt install -y xorriso mtools libmpfr-dev texinfo libfuse-dev
apt install -y gcc-multilib clang-tidy uuid-dev

#!/bin/bash

# Debian or Ubuntu based distros
if [ -f /etc/debian-release ]; then
        sudo apt update
        sudo apt upgrade

        sudo apt install nasm gcc make binutils grub-pc-bin xorriso mtools imagemagick git qemu qemu-kvm

elif [ -f /etc/ubuntu-release ]; then
        sudo apt update
        sudo apt upgrade

        sudo apt install nasm gcc make binutils grub-pc-bin xorriso mtools imagemagick git qemu qemu-kvm

# RedHat or RedHat based distros
elif [ -f /etc/redhat-release ]; then
        cat /etc/redhat-release

        sudo dnf update
        sudo dnf upgrade

        sudo dnf install nasm gcc make binutils xorriso mtools ImageMagick git qemu qemu-kvm

elif [ -f /etc/fedora-release ]; then
        cat /etc/fedora-release

        sudo dnf update
        sudo dnf upgrade

        sudo dnf install nasm gcc make binutils xorriso mtools ImageMagick git qemu qemu-kvm

elif [ -f /etc/centos-release ]; then
        cat /etc/centos-release

        sudo dnf update
        sudo dnf upgrade

        sudo dnf install nasm gcc make binutils xorriso mtools ImageMagick git qemu qemu-kvm

# Arch or Arch based distros
elif [ -f /etc/arch-release ]; then
        cat /etc/redhat-release

        sudo pacman -Syu

        sudo pacman -S base-devel bison flex mpc mpfr texinfo
fi

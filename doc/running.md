# Running

This guide provides instructions for running skiftOS on QEMU or real hardware using the HDD file.

## On QEMU

### 1. Obtain OVMF firmware

- Using the package manager:
  - Arch Linux: `pacman -S edk2-ovmf`
  - Ubuntu: `apt install ovmf`
  - File location: `/usr/share/OVMF/x64/OVMF.fd`
- Alternatively, [Download OVMF](https://retrage.github.io/edk2-nightly)

### 2. Obtain QEMU

- Using the package manager:
  - Arch Linux: `pacman -S qemu`
  - Ubuntu: `apt install qemu-system-x86`

### 3. Run QEMU

Execute the following command:

```bash
$ qemu-system-x86_64 -bios <ovmf file> -drive format=raw,file=<hdd file> -m 512M -serial stdio -enable-kvm
```

Note: Replace `<ovmf file>` with the appropriate OVMF firmware file and `<hdd file>` with the HDD file you downloaded or builded.

## On Real Hardware

> **⚠ Warning:** Running skiftOS on real hardware is not recommended. It is not ready for production use and may damage your computer.

If still determined, flashing the HDD file to a USB drive can be attempted:

```bash
$ dd if=image.hdd of=/dev/sdX bs=4M status=progress
```
Note: Replace `/dev/sdX` with the appropriate destination device.

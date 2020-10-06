# The skift operating system todo list

## Project

### Transition from C to C++
 - [x] Update to GCC 10
 - [x] Update binutils
 - [x] Update the make file to support C++
 - [x] Rename all files from .c to .cpp
 - [ ] Rewrite the content of libsystem/utils

## Build
 - [ ] Improve make clean, don't remove rendered icons. 

## Kernel

### ACPI
 - [x] Get the acpi table from the bootloader
    (https://uefi.org/sites/default/files/resources/ACPI_6_3_final_Jan30.pdf)
 - [ ] Implement a fallback
 - [x] Port lai
    (https://github.com/qword-os/lai) (same callback are still missing)

### VirtIO
 - [ ] VirtIO bus
    (https://wiki.osdev.org/Virtio)
    (https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-3200007)
    (https://github.com/doug65536/dgos/tree/master/kernel/device/virtio-base)


### x86 platform
 - [ ] Enable localapic and ioapic
 - [ ] Support for SMP
 - [ ] Map the kernel to the higher half of the memory

### Networking
 - [ ] VirtIO network device
 - [ ] Protocols
 (https://wiki.osdev.org/Network_Stack#Networking_protocols)
    - [ ] Implement ARP
    - [ ] Implement IP
    - [ ] Implement ICMP
    - [ ] Implement UDP
    - [ ] Implement DHCP
    - [ ] Implement DNS
    - [ ] Implement TCP
    - [ ] Implement SSL/TLS
    - [ ] Implement HTTP
    - [ ] Implement TelNet

### Graphics
 - [ ] VirtIO Graphic device
 - [ ] VirtGL support
    (https://studiopixl.com/2017-08-27/3d-acceleration-using-virtio.html)
    (https://github.com/Keenuts/virtio-gpu-documentation/blob/master/src/virtio-gpu.md)

## Userspace

- [ ] IPC framework
 - [ ] IPC interface description language (toolbox/ipc-compiler.py)
 - [ ] IPC library (libraries/libipc/)
 
- [ ] init/sevice-manager (OpenRC/launchd/upstart/systemd like)

## Application

### Http Server (Required Kernel/Networking)

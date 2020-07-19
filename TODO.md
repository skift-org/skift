# The skift operating system todo list

## Project

### Transition from C to C++
 - [ ] Update to GCC 10
 - [ ] Update binutils
 - [ ] Update the make file to support C++
 - [ ] Rename all files from .c to .cpp
 - [ ] Rewrite the content of libsystem/utils

## Kernel

### ACPI
 - [ ] Get the acpi table from the bootloader
 - [ ] Implement a fallback
 - [ ] Port lai
    (https://github.com/qword-os/lai)

### VirtIO
 - [ ] VirtIO bus
    (https://docs.oasis-open.org/virtio/virtio/v1.1/csprd01/virtio-v1.1-csprd01.html#x1-3001r1)

### x86 platform
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

## Application

### Http Server (Required Kernel/Networking)

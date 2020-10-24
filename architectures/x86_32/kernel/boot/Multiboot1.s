;; --- multiboot-1 header. -------------------------------------------------- ;;

MULTIBOOT_PAGE_ALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MULTIBOOT_MEMORY_INFO  equ  1 << 1           ; provide memory map
MULTIBOOT_VIDEO_MODE  equ  1 << 2            ; provide a video mode
MULTIBOOT_MAGIC    equ  0x1BADB002           ; 'magic number' lets bootloader find the header

multiboot_flags    equ  MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE; this is the Multiboot 'flag' field
multiboot_checksum equ -(MULTIBOOT_MAGIC + multiboot_flags)   ; checksum of above, to prove we are multiboot

; Declare a multiboot header that marks the program as a kernel.
section .multiboot1
align 4
    dd MULTIBOOT_MAGIC
    dd multiboot_flags
    dd multiboot_checksum

    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000
    dd 0x00000000

    dd 0x00000000
    dd 0
    dd 0
    dd 32

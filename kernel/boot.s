;; Copyright © 2018-2020 N. Van Bossuyt.                                      ;;
;; This code is licensed under the MIT License.                               ;;
;; See: LICENSE.md                                                            ;;

;; boots.s: the HJERT kernel entry point from the bootloader.                 ;;

;; --- multiboot-1 header. -------------------------------------------------- ;;

MULTIBOOT_PAGE_ALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MULTIBOOT_MEMORY_INFO  equ  1 << 1            ; provide memory map
MULTIBOOT_VIDEO_MODE  equ  1 << 2            ; provide memory map
MULTIBOOT_MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header

multiboot_flags    equ  MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO  ; this is the Multiboot 'flag' field
multiboot_checksum equ -(MULTIBOOT_MAGIC + multiboot_flags)   ; checksum of above, to prove we are multiboot
 
; Declare a multiboot header that marks the program as a kernel.
section .multiboot
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
	dd 800
	dd 600
	dd 32

;; --- multiboot-2 header. -------------------------------------------------- ;;

MULTIBOOT2_HEADER_MAGIC equ 0xe85250d6
MULTIBOOT2_ARCHITECTURE_I386 equ 0
MULTIBOOT2_HEADER_LENGHT equ __multiboot_header_end - __multiboot_header_start
MULTIBOOT2_CHECKSUM equ -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + MULTIBOOT2_HEADER_LENGHT)

MULTIBOOT2_HEADER_TAG_END equ 0
MULTIBOOT2_HEADER_TAG_INFORMATION_REQUEST equ 1
MULTIBOOT2_HEADER_TAG_ADDRESS equ 2
MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS equ 3
MULTIBOOT2_HEADER_TAG_CONSOLE_FLAGS equ 4
MULTIBOOT2_HEADER_TAG_FRAMEBUFFER equ 5
MULTIBOOT2_HEADER_TAG_MODULE_ALIGN equ 6
MULTIBOOT2_HEADER_TAG_EFI_BS equ 7
MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS_EFI32 equ 8
MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS_EFI64 equ 9
MULTIBOOT2_HEADER_TAG_RELOCATABLE equ 10

section .multiboot
__multiboot_header_start:

align 4
    dd MULTIBOOT2_HEADER_MAGIC
    dd MULTIBOOT2_ARCHITECTURE_I386
    dd MULTIBOOT2_HEADER_LENGHT
    dd MULTIBOOT2_CHECKSUM

; END HEADER TAG
    dw MULTIBOOT2_HEADER_TAG_END
    dw 0
    dw 8

__multiboot_header_end:
 
;; --- kernel stack --------------------------------------------------------- ;;

section .bss
align 16

global __stack_bottom
global __stack_top
__stack_bottom:
resb 16384 ; 16 KiB
__stack_top:

;; --- kernel entry point --------------------------------------------------- ;;

section .text
global _kstart:function (_kstart.end - _kstart)
_kstart:
	cli
	cld
	; To set up a stack, we set the esp register to point to the top of our
	; stack (as it grows downwards on x86 systems).
	mov esp, __stack_top
 
	push eax ; Push the multiboot magic
	push ebx ; Push the multiboot header adress.
	
	extern kmain
	call kmain

	cli
.hang:	hlt
	jmp .hang
.end:
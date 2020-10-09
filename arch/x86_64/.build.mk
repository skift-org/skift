CC:=x86_64-pc-skift-gcc
CXX:=x86_64-pc-skift-g++
LD:=x86_64-pc-skift-ld
AR:=x86_64-pc-skift-ar
AS:=nasm
ASFLAGS:=-f elf64

KERNEL_SOURCES += $(wildcard arch/x86/kernel/*.cpp)
KERNEL_ASSEMBLY_SOURCES += $(wildcard arch/x86/kernel/*.s)

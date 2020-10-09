CC:=i686-pc-skift-gcc
CXX:=i686-pc-skift-g++
LD:=i686-pc-skift-ld
AR:=i686-pc-skift-ar
AS=nasm
ASFLAGS=-f elf32

KERNEL_SOURCES += $(wildcard arch/x86/kernel/*.cpp)
KERNEL_ASSEMBLY_SOURCES += $(wildcard arch/x86/kernel/*.s)

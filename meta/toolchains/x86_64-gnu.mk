CC:=x86_64-pc-skift-gcc
CXX:=x86_64-pc-skift-g++
LD:=x86_64-pc-skift-ld
LDFLAGS:= \
	--sysroot=$(SYSROOT)
AR:=x86_64-pc-skift-ar
ARFLAGS:=rcs

AS:=nasm
ASFLAGS:=-f elf64

STRIP:=x86_64-pc-skift-strip

KERNEL_CXXFLAGS += \
	-z max-page-size=0x1000 	   \
	-fno-pic                       \
	-mno-sse                       \
	-mno-sse2                      \
	-mno-mmx                       \
	-mno-80387                     \
	-mno-red-zone                  \
	-mcmodel=kernel                \
	-ffreestanding                 \
	-fno-stack-protector           \
	-fno-omit-frame-pointer

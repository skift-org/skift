CC:=x86_64-pc-skift-gcc
CXX:=x86_64-pc-skift-g++
LD:=x86_64-pc-skift-ld
AR:=x86_64-pc-skift-ar
AS:=nasm
ASFLAGS:=-f elf64

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

KERNEL_LDFLAGS += -z max-page-size=0x1000

KERNEL_SOURCES += $(wildcard kernel/archs/x86/*.cpp)

KERNEL_ASSEMBLY_SOURCES += $(wildcard kernel/archs/x86/*.s)

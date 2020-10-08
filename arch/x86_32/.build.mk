CC:=i686-pc-skift-gcc
CXX:=i686-pc-skift-g++
LD:=i686-pc-skift-ld
AR:=i686-pc-skift-ar
AS=nasm

KERNEL_SOURCES += \
	$(wildcard arch/x86/*.cpp) \
	$(wildcard arch/x86_32/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard arch/x86/*.s) \
	$(wildcard arch/x86_32/*.s)

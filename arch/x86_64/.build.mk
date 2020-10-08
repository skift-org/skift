CC:=x86_64-pc-skift-gcc
CXX:=x86_64-pc-skift-g++
LD:=x86_64-pc-skift-ld
AR:=x86_64-pc-skift-ar
AS=nasm

KERNEL_SOURCES += \
	$(wildcard arch/x86/*.cpp) \
	$(wildcard arch/x86_64/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard arch/x86/*.s) \
	$(wildcard arch/x86_64/*.s)

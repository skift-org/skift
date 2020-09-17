KERNEL_SOURCES += \
	$(wildcard arch/x86/*.cpp) \
	$(wildcard arch/x86_64/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard arch/x86/*.s) \
	$(wildcard arch/x86_64/*.s)

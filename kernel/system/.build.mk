KERNEL_SOURCES += \
	$(wildcard kernel/system/*.cpp) \
	$(wildcard kernel/system/*/*.cpp) \
	$(wildcard kernel/system/*/*/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard kernel/system/*.s) \
	$(wildcard kernel/system/*/*.s)
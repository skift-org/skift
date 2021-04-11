CRTS= \
	$(BUILD_DIRECTORY_LIBS)/crt0.o \
	$(BUILD_DIRECTORY_LIBS)/crti.o \
	$(BUILD_DIRECTORY_LIBS)/crtn.o

-include kernel/archs/$(CONFIG_ARCH)/.build.mk

KERNEL_SOURCES += \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/kernel/*.cpp) \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/kernel/*/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/kernel/*.s) \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/kernel/*/*.s)

list-src:
	@echo $(KERNEL_SOURCES)

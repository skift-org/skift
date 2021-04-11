CRTS= \
	$(BUILD_DIRECTORY_LIBS)/crt0.o \
	$(BUILD_DIRECTORY_LIBS)/crti.o \
	$(BUILD_DIRECTORY_LIBS)/crtn.o

-include kernel/archs/$(CONFIG_ARCH)/.build.mk

KERNEL_SOURCES += \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/*.cpp) \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/*/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/*.s) \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/*/*.s)

list-src:
	@echo $(KERNEL_SOURCES)

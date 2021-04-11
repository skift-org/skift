CRTS= \
	$(BUILD_DIRECTORY_LIBS)/crt0.o \
	$(BUILD_DIRECTORY_LIBS)/crti.o \
	$(BUILD_DIRECTORY_LIBS)/crtn.o

-include kernel/archs/$(CONFIG_ARCH)/.build.mk

$(BUILD_DIRECTORY_LIBS)/crt0.o: kernel/archs/$(CONFIG_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crti.o: kernel/archs/$(CONFIG_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crtn.o: kernel/archs/$(CONFIG_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

KERNEL_SOURCES += \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/kernel/*.cpp) \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/kernel/*/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/kernel/*.s) \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/kernel/*/*.s)

list-src:
	@echo $(KERNEL_SOURCES)

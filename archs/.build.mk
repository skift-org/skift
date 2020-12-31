CRTS= \
	$(BUILD_DIRECTORY_LIBS)/crt0.o \
	$(BUILD_DIRECTORY_LIBS)/crti.o \
	$(BUILD_DIRECTORY_LIBS)/crtn.o

-include archs/$(CONFIG_ARCH)/.build.mk

$(BUILD_DIRECTORY_LIBS)/crt0.o: archs/$(CONFIG_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crti.o: archs/$(CONFIG_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crtn.o: archs/$(CONFIG_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

KERNEL_SOURCES += \
	$(wildcard archs/$(CONFIG_ARCH)/kernel/*.cpp) \
	$(wildcard archs/$(CONFIG_ARCH)/kernel/*/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard archs/$(CONFIG_ARCH)/kernel/*.s) \
	$(wildcard archs/$(CONFIG_ARCH)/kernel/*/*.s)

list-src:
	@echo $(KERNEL_SOURCES)

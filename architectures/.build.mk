CRTS= \
	$(BUILD_DIRECTORY_LIBS)/crt0.o \
	$(BUILD_DIRECTORY_LIBS)/crti.o \
	$(BUILD_DIRECTORY_LIBS)/crtn.o

-include architectures/$(BUILD_ARCH)/.build.mk

$(BUILD_DIRECTORY_LIBS)/crt0.o: architectures/$(BUILD_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crti.o: architectures/$(BUILD_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crtn.o: architectures/$(BUILD_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

KERNEL_SOURCES += \
	$(wildcard architectures/$(BUILD_ARCH)/kernel/*.cpp) \
	$(wildcard architectures/$(BUILD_ARCH)/kernel/*/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard architectures/$(BUILD_ARCH)/kernel/*.s) \
	$(wildcard architectures/$(BUILD_ARCH)/kernel/*/*.s)

list-src:
	@echo $(KERNEL_SOURCES)

-include arch/$(BUILD_ARCH)/.build.mk

CRTS= \
	$(BUILD_DIRECTORY_LIBS)/crt0.o \
	$(BUILD_DIRECTORY_LIBS)/crti.o \
	$(BUILD_DIRECTORY_LIBS)/crtn.o

$(BUILD_DIRECTORY_LIBS)/crt0.o: arch/$(BUILD_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crti.o: arch/$(BUILD_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crtn.o: arch/$(BUILD_ARCH)/crts/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^


KERNEL_SOURCES += \
	$(wildcard arch/$(BUILD_ARCH)/kernel/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard arch/$(BUILD_ARCH)/kernel/*.s)

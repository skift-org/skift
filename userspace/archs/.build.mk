CRTS= \
	$(BUILD_DIRECTORY_LIBS)/crt0.o \
	$(BUILD_DIRECTORY_LIBS)/crti.o \
	$(BUILD_DIRECTORY_LIBS)/crtn.o

$(BUILD_DIRECTORY_LIBS)/crt0.o: userspace/archs/$(CONFIG_ARCH)/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crti.o: userspace/archs/$(CONFIG_ARCH)/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^

$(BUILD_DIRECTORY_LIBS)/crtn.o: userspace/archs/$(CONFIG_ARCH)/crt0.s
	$(DIRECTORY_GUARD)
	@echo [AS] $^
	@$(AS) $(ASFLAGS) -o $@ $^
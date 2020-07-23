KERNEL_SOURCES = \
	$(wildcard kernel/*.c) \
	$(wildcard kernel/*/*.c) \
	$(wildcard arch/x86/*.c)

KERNEL_ASSEMBLY_SOURCES = \
	$(wildcard kernel/*.s) \
	$(wildcard kernel/*/*.s) \
	$(wildcard arch/*/*.s)

KERNEL_LIBRARIES_SOURCES = \
	$(wildcard libraries/libfile/*.c) \
	$(wildcard libraries/libjson/*.c) \
	$(wildcard libraries/libsystem/*.c) \
	$(wildcard libraries/libsystem/io/*.c) \
	$(wildcard libraries/libsystem/unicode/*.c) \
	$(wildcard libraries/libsystem/process/*.c) \
	$(wildcard libraries/libsystem/math/*.c) \
	$(wildcard libraries/libsystem/utils/*.c)

KERNEL_BINARY = $(BOOTROOT)/boot/kernel.bin

KERNEL_OBJECTS = \
	$(patsubst %.c, $(BUILD_DIRECTORY)/%.o, $(KERNEL_SOURCES)) \
	$(patsubst %.s, $(BUILD_DIRECTORY)/%.s.o, $(KERNEL_ASSEMBLY_SOURCES)) \
	$(patsubst libraries/%.c, $(BUILD_DIRECTORY)/kernel/%.o, $(KERNEL_LIBRARIES_SOURCES))

OBJECTS += $(KERNEL_OBJECTS)

$(BUILD_DIRECTORY)/kernel/%.o: libraries/%.c
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CC] $<
	@$(CC) $(CFLAGS) -ffreestanding -nostdlib -c -o $@ $<

$(BUILD_DIRECTORY)/kernel/%.o: kernel/%.c
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CC] $<
	@$(CC) $(CFLAGS) -ffreestanding -nostdlib -c -o $@ $<

$(BUILD_DIRECTORY)/arch/%.o: arch/%.c
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CC] $<
	@$(CC) $(CFLAGS) -ffreestanding -nostdlib -c -o $@ $<

$(BUILD_DIRECTORY)/arch/%.s.o: arch/%.s
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [AS] $<
	@$(AS) $(ASFLAGS) $^ -o $@

$(KERNEL_BINARY): $(KERNEL_OBJECTS)
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [LD] $(KERNEL_BINARY)
	@$(CC) $(LDFLAGS) -T arch/x86/link.ld -o $@ -ffreestanding $^ -nostdlib -lgcc

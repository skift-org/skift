KEYBOARD_LAYOUT="fr_be"

KERNEL_SOURCES = \
	$(wildcard kernel/*.cpp) \
	$(wildcard kernel/*/*.cpp) \
	$(wildcard arch/x86/*.cpp)

KERNEL_ASSEMBLY_SOURCES = \
	$(wildcard kernel/*.s) \
	$(wildcard kernel/*/*.s) \
	$(wildcard arch/*/*.s)

KERNEL_LIBRARIES_SOURCES = \
	$(wildcard libraries/libfile/*.cpp) \
	$(wildcard libraries/libjson/*.cpp) \
	$(wildcard libraries/libsystem/*.cpp) \
	$(wildcard libraries/libsystem/io/*.cpp) \
	$(wildcard libraries/libsystem/unicode/*.cpp) \
	$(wildcard libraries/libsystem/process/*.cpp) \
	$(wildcard libraries/libsystem/math/*.cpp) \
	$(wildcard libraries/libsystem/utils/*.cpp) \
	$(wildcard libraries/libsystem/core/*.cpp) \
	$(wildcard libraries/libsystem/thread/*.cpp) \
	$(wildcard libraries/libsystem/system/*.cpp)

KERNEL_BINARY = $(BOOTROOT)/boot/kernel.bin

KERNEL_OBJECTS = \
	$(patsubst %.cpp, $(BUILD_DIRECTORY)/%.o, $(KERNEL_SOURCES)) \
	$(patsubst %.s, $(BUILD_DIRECTORY)/%.s.o, $(KERNEL_ASSEMBLY_SOURCES)) \
	$(patsubst libraries/%.cpp, $(BUILD_DIRECTORY)/kernel/%.o, $(KERNEL_LIBRARIES_SOURCES))

OBJECTS += $(KERNEL_OBJECTS)

$(BUILD_DIRECTORY)/kernel/%.o: libraries/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CC] $<
	@$(CC) $(CFLAGS) -ffreestanding -nostdlib -c -o $@ $<

$(BUILD_DIRECTORY)/kernel/%.o: kernel/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CC] $<
	@$(CC) -DKEYBOARD_LAYOUT=\"/System/Keyboards/$(KEYBOARD_LAYOUT).kmap\" $(CFLAGS) -ffreestanding -nostdlib -c -o $@ $<

$(BUILD_DIRECTORY)/arch/%.o: arch/%.cpp
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

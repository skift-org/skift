KERNEL_SOURCES += \
	$(wildcard kernel/kernel/*.cpp) \
	$(wildcard kernel/kernel/*/*.cpp) \
	$(wildcard kernel/kernel/*/*/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard kernel/kernel/*.s) \
	$(wildcard kernel/kernel/*/*.s)

KERNEL_LIBRARIES_SOURCES = \
	$(wildcard userspace/libraries/libc/string.cpp) \
	$(wildcard userspace/libraries/libc/assert.cpp) \
	$(wildcard userspace/libraries/libc/ctype.cpp) \
	$(wildcard userspace/libraries/libc/skift/NumberFormatter.cpp) \
	$(wildcard userspace/libraries/libc/skift/Printf.cpp) \
	$(wildcard userspace/libraries/libc/skift/Time.cpp) \
	$(wildcard userspace/libraries/libc/stdlib/allocator.cpp) \
	$(wildcard userspace/libraries/libc/stdio/sprintf.cpp) \
	$(wildcard userspace/libraries/libc/cxx/new-delete.cpp) \
	$(wildcard userspace/libraries/libfile/TARArchive.cpp) \
	$(wildcard userspace/libraries/libsystem/json/*.cpp) \
	$(wildcard userspace/libraries/libsystem/*.cpp) \
	$(wildcard userspace/libraries/libsystem/io/*.cpp) \
	$(wildcard userspace/libraries/libsystem/unicode/*.cpp) \
	$(wildcard userspace/libraries/libsystem/process/*.cpp) \
	$(wildcard userspace/libraries/libsystem/utils/*.cpp) \
	$(wildcard userspace/libraries/libsystem/core/*.cpp) \
	$(wildcard userspace/libraries/libsystem/thread/*.cpp) \
	$(wildcard userspace/libraries/libsystem/system/*.cpp) \

KERNEL_BINARY = $(BUILDROOT)/kernel.bin

KERNEL_OBJECTS = \
	$(patsubst %.cpp, $(BUILDROOT)/%.o, $(KERNEL_SOURCES)) \
	$(patsubst %.s, $(BUILDROOT)/%.s.o, $(KERNEL_ASSEMBLY_SOURCES)) \
	$(patsubst userspace/libraries/%.cpp, $(BUILDROOT)/kernel/%.o, $(KERNEL_LIBRARIES_SOURCES))

KERNEL_CXXFLAGS += \
	$(CXXFLAGS) 	\
	-fno-rtti \
	-fno-exceptions \
	-ffreestanding \
	-nostdlib \
	-D__KERNEL__ \
	-DCONFIG_KEYBOARD_LAYOUT=\""${CONFIG_KEYBOARD_LAYOUT}"\"

OBJECTS += $(KERNEL_OBJECTS)

$(BUILDROOT)/kernel/%.o: userspace/libraries/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -c -o $@ $<

$(BUILDROOT)/kernel/kernel/%.o: kernel/kernel/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -ffreestanding -nostdlib -c -o $@ $<

$(BUILDROOT)/kernel/archs/%.o: kernel/archs/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -c -o $@ $<

$(BUILDROOT)/kernel/archs/%.s.o: kernel/archs/%.s
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [AS] $<
	@$(AS) $(ASFLAGS) $^ -o $@

$(KERNEL_BINARY): $(KERNEL_OBJECTS)
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [LD] $(KERNEL_BINARY)
	@$(CXX) $(LDFLAGS) $(KERNEL_LDFLAGS) -T kernel/archs/$(CONFIG_ARCH)/link.ld -o $@ -ffreestanding $^ -nostdlib -lgcc

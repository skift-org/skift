KERNEL_SOURCES += \
	$(wildcard kernel/*.cpp) \
	$(wildcard kernel/*/*.cpp) \
	$(wildcard kernel/*/*/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard kernel/*.s) \
	$(wildcard kernel/*/*.s)

KERNEL_LIBRARIES_SOURCES = \
	$(wildcard libraries/libc/string.cpp) \
	$(wildcard libraries/libc/assert.cpp) \
	$(wildcard libraries/libc/ctype.cpp) \
	$(wildcard libraries/libc/skift/NumberFormatter.cpp) \
	$(wildcard libraries/libc/skift/Printf.cpp) \
	$(wildcard libraries/libc/skift/Time.cpp) \
	$(wildcard libraries/libc/stdlib/allocator.cpp) \
	$(wildcard libraries/libc/stdio/sprintf.cpp) \
	$(wildcard libraries/libc/cxx/new-delete.cpp) \
	$(wildcard libraries/libfile/TARArchive.cpp) \
	$(wildcard libraries/libsystem/json/*.cpp) \
	$(wildcard libraries/libsystem/*.cpp) \
	$(wildcard libraries/libsystem/io/*.cpp) \
	$(wildcard libraries/libsystem/unicode/*.cpp) \
	$(wildcard libraries/libsystem/process/*.cpp) \
	$(wildcard libraries/libsystem/utils/*.cpp) \
	$(wildcard libraries/libsystem/core/*.cpp) \
	$(wildcard libraries/libsystem/thread/*.cpp) \
	$(wildcard libraries/libsystem/system/*.cpp) \

KERNEL_BINARY = $(CONFIG_BUILD_DIRECTORY)/kernel.bin

KERNEL_OBJECTS = \
	$(patsubst %.cpp, $(CONFIG_BUILD_DIRECTORY)/%.o, $(KERNEL_SOURCES)) \
	$(patsubst %.s, $(CONFIG_BUILD_DIRECTORY)/%.s.o, $(KERNEL_ASSEMBLY_SOURCES)) \
	$(patsubst libraries/%.cpp, $(CONFIG_BUILD_DIRECTORY)/kernel/%.o, $(KERNEL_LIBRARIES_SOURCES))

KERNEL_CXXFLAGS += \
	$(CXXFLAGS) 	\
	-fno-rtti \
	-fno-exceptions \
	-ffreestanding \
	-nostdlib \
	-D__KERNEL__ \
	-DCONFIG_KEYBOARD_LAYOUT=\""${CONFIG_KEYBOARD_LAYOUT}"\"

OBJECTS += $(KERNEL_OBJECTS)

$(CONFIG_BUILD_DIRECTORY)/kernel/%.o: libraries/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -c -o $@ $<

$(CONFIG_BUILD_DIRECTORY)/kernel/%.o: kernel/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -ffreestanding -nostdlib -c -o $@ $<

$(CONFIG_BUILD_DIRECTORY)/archs/%.o: archs/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -c -o $@ $<

$(CONFIG_BUILD_DIRECTORY)/archs/%.s.o: archs/%.s
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [AS] $<
	@$(AS) $(ASFLAGS) $^ -o $@

$(KERNEL_BINARY): $(KERNEL_OBJECTS)
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [LD] $(KERNEL_BINARY)
	@$(CXX) $(LDFLAGS) $(KERNEL_LDFLAGS) -T archs/$(CONFIG_ARCH)/link.ld -o $@ -ffreestanding $^ -nostdlib -lgcc

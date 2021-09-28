include kernel/archs/.build.mk
include kernel/modules/.build.mk
include kernel/system/.build.mk

KERNEL_LIBRARIES_SOURCES = \
	$(wildcard userspace/libs/libc/string.cpp) \
	$(wildcard userspace/libs/libc/assert.cpp) \
	$(wildcard userspace/libs/libc/ctype.cpp) \
	$(wildcard userspace/libs/libc/skift/NumberFormatter.cpp) \
	$(wildcard userspace/libs/libc/skift/Printf.cpp) \
	$(wildcard userspace/libs/libc/skift/Time.cpp) \
	$(wildcard userspace/libs/libc/stdlib/allocator.cpp) \
	$(wildcard userspace/libs/libc/stdio/sprintf.cpp) \
	$(wildcard userspace/libs/libc/cxx/new-delete.cpp) \
	$(wildcard userspace/libs/libfile/TARArchive.cpp) \
	$(wildcard userspace/libs/libsystem/json/*.cpp) \
	$(wildcard userspace/libs/libsystem/*.cpp) \
	$(wildcard userspace/libs/libsystem/io/*.cpp) \
	$(wildcard userspace/libs/libsystem/unicode/*.cpp) \
	$(wildcard userspace/libs/libsystem/process/*.cpp) \
	$(wildcard userspace/libs/libsystem/utils/*.cpp) \
	$(wildcard userspace/libs/libsystem/core/*.cpp) \
	$(wildcard userspace/libs/libsystem/thread/*.cpp) \
	$(wildcard userspace/libs/libsystem/system/*.cpp) \
	$(wildcard userspace/libs/libio/Format.cpp) \

KERNEL_BINARY = $(BUILDROOT)/kernel.bin

KERNEL_OBJECTS = \
	$(patsubst %.cpp, $(BUILDROOT)/%.o, $(KERNEL_SOURCES)) \
	$(patsubst %.s, $(BUILDROOT)/%.s.o, $(KERNEL_ASSEMBLY_SOURCES)) \
	$(patsubst userspace/libs/%.cpp, $(BUILDROOT)/kernel/%.o, $(KERNEL_LIBRARIES_SOURCES))

KERNEL_CXXFLAGS += \
	$(CXXFLAGS) 	\
	-fno-rtti \
	-fno-exceptions \
	-ffreestanding \
	-nostdlib \
	-D__KERNEL__ \
	-DCONFIG_KEYBOARD_LAYOUT=\""${CONFIG_KEYBOARD_LAYOUT}"\"

OBJECTS += $(KERNEL_OBJECTS)

$(BUILDROOT)/kernel/%.o: userspace/libs/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -c -o $@ $<

$(BUILDROOT)/kernel/system/%.o: kernel/system/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -ffreestanding -nostdlib -c -o $@ $<

$(KERNEL_BINARY): $(KERNEL_OBJECTS)
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [LD] $(KERNEL_BINARY)
	@$(CXX) $(LDFLAGS) $(KERNEL_LDFLAGS) -T kernel/archs/$(CONFIG_ARCH)/link.ld -o $@ -ffreestanding $^ -nostdlib -lgcc

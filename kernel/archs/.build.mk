-include kernel/archs/$(CONFIG_ARCH)/.build.mk

KERNEL_SOURCES += \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/*.cpp) \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/*/*.cpp)

KERNEL_ASSEMBLY_SOURCES += \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/*.s) \
	$(wildcard kernel/archs/$(CONFIG_ARCH)/*/*.s)

$(BUILDROOT)/kernel/archs/%.o: kernel/archs/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -c -o $@ $<

$(BUILDROOT)/kernel/archs/%.s.o: kernel/archs/%.s
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [AS] $<
	@$(AS) $(ASFLAGS) $^ -o $@

KERNEL_SOURCES += \
	$(wildcard kernel/modules/*.cpp) \
	$(wildcard kernel/modules/*/*.cpp)

$(BUILDROOT)/kernel/modules/%.o: kernel/modules/%.cpp
	$(DIRECTORY_GUARD)
	@echo [KERNEL] [CXX] $<
	@$(CXX) $(KERNEL_CXXFLAGS) -c -o $@ $<
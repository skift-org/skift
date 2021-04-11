ABI_HEADERS = \
	$(wildcard userspace/libraries/abi/*.h) \
	$(wildcard userspace/libraries/abi/*/*.h)

LIBUTILS_HEADERS = \
	$(wildcard userspace/libraries/libutils/*.h) \
	$(wildcard userspace/libraries/libutils/*/*.h)

HEADERS += $(patsubst userspace/libraries/%, $(BUILD_DIRECTORY_INCLUDE)/%, $(ABI_HEADERS) $(LIBUTILS_HEADERS))

define LIB_TEMPLATE =

$(1)_ARCHIVE ?= $(BUILD_DIRECTORY_LIBS)/lib$($(1)_NAME).a

$(1)_SOURCES += \
	$$(wildcard userspace/libraries/lib$($(1)_NAME)/*.cpp) \
	$$(wildcard userspace/libraries/lib$($(1)_NAME)/*/*.cpp)

$(1)_C_SOURCES += \
	$$(wildcard userspace/libraries/lib$($(1)_NAME)/*.c) \
	$$(wildcard userspace/libraries/lib$($(1)_NAME)/*/*.c)

$(1)_ASSEMBLY_SOURCES += \
	$$(wildcard userspace/libraries/lib$($(1)_NAME)/*.s) \
	$$(wildcard userspace/libraries/lib$($(1)_NAME)/*/*.s)

$(1)_OBJECTS = $$(patsubst userspace/libraries/%.cpp, $(BUILDROOT)/userspace/libraries/%.o, $$($(1)_SOURCES)) \
			   $$(patsubst userspace/libraries/%.c, $(BUILDROOT)/userspace/libraries/%.c.o, $$($(1)_C_SOURCES)) \
			   $$(patsubst userspace/libraries/%.s, $(BUILDROOT)/userspace/libraries/%.s.o, $$($(1)_ASSEMBLY_SOURCES))

$(1)_HEADERS = \
	$$(wildcard userspace/libraries/lib$($(1)_NAME)/*.h) \
	$$(wildcard userspace/libraries/lib$($(1)_NAME)/*/*.h)

TARGETS += $$($(1)_ARCHIVE)
OBJECTS += $$($(1)_OBJECTS)

# Special case for libc to copy the headers to the right location without the libc prefix.
ifneq ($(1), C)
HEADERS += $$(patsubst userspace/libraries/%, $(BUILD_DIRECTORY_INCLUDE)/%, $$($(1)_HEADERS))
else
HEADERS += $$(patsubst userspace/libraries/libc/%, $(BUILD_DIRECTORY_INCLUDE)/%, $$($(1)_HEADERS))
endif

$$($(1)_ARCHIVE): $$($(1)_OBJECTS)
	$$(DIRECTORY_GUARD)
	@echo [LIB$(1)] [AR] $$@
	@$(AR) $(ARFLAGS) $$@ $$^

$(BUILDROOT)/userspace/libraries/lib$($(1)_NAME)/%.o: userspace/libraries/lib$($(1)_NAME)/%.cpp
	$$(DIRECTORY_GUARD)
	@echo [LIB$(1)] [CXX] $$<
	@$(CXX) $(CXXFLAGS) $($(1)_CXXFLAGS) -c -o $$@ $$<

$(BUILDROOT)/userspace/libraries/lib$($(1)_NAME)/%.c.o: userspace/libraries/lib$($(1)_NAME)/%.c
	$$(DIRECTORY_GUARD)
	@echo [LIB$(1)] [CC] $$<
	@$(CC) $(CFLAGS) $($(1)_CFLAGS) -c -o $$@ $$<

$(BUILDROOT)/userspace/libraries/lib$($(1)_NAME)/%.s.o: userspace/libraries/lib$($(1)_NAME)/%.s
	$$(DIRECTORY_GUARD)
	@echo [LIB$(1)] [AS] $$<
	@$(AS) $(ASFLAGS) $$^ -o $$@

endef

$(BUILD_DIRECTORY_INCLUDE)/%.h: userspace/libraries/%.h
	$(DIRECTORY_GUARD)
	cp $< $@

$(BUILD_DIRECTORY_INCLUDE)/%.h: userspace/libraries/libc/%.h
	$(DIRECTORY_GUARD)
	cp $< $@

-include userspace/libraries/*/.build.mk
$(foreach lib, $(LIBS), $(eval $(call LIB_TEMPLATE,$(lib))))

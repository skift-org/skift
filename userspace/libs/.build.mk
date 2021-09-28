ABI_HEADERS = \
	$(wildcard userspace/libs/abi/*.h) \
	$(wildcard userspace/libs/abi/*/*.h)

LIBUTILS_HEADERS = \
	$(wildcard userspace/libs/libutils/*.h) \
	$(wildcard userspace/libs/libutils/*/*.h)

HEADERS += $(patsubst userspace/libs/%, $(BUILD_DIRECTORY_INCLUDE)/%, $(ABI_HEADERS) $(LIBUTILS_HEADERS))

define LIB_TEMPLATE =

$(1)_ARCHIVE ?= $(BUILD_DIRECTORY_LIBS)/lib$($(1)_NAME).a

$(1)_SOURCES += \
	$$(wildcard userspace/libs/lib$($(1)_NAME)/*.cpp) \
	$$(wildcard userspace/libs/lib$($(1)_NAME)/*/*.cpp) \
	$$(wildcard userspace/libs/lib$($(1)_NAME)/*.cppm) \
	$$(wildcard userspace/libs/lib$($(1)_NAME)/*/*.cppm) \
	$$(wildcard userspace/libs/lib$($(1)_NAME)/*.c) \
	$$(wildcard userspace/libs/lib$($(1)_NAME)/*/*.c) \

$(1)_OBJECTS = $$(patsubst userspace/libs/%, $(BUILDROOT)/userspace/libs/%.o, $$($(1)_SOURCES))

$(1)_HEADERS = \
	$$(wildcard userspace/libs/lib$($(1)_NAME)/*.h) \
	$$(wildcard userspace/libs/lib$($(1)_NAME)/*/*.h)

TARGETS += $$($(1)_ARCHIVE)
OBJECTS += $$($(1)_OBJECTS)

# Special case for libc to copy the headers to the right location without the libc prefix.
ifneq ($(1), C)
HEADERS += $$(patsubst userspace/libs/%, $(BUILD_DIRECTORY_INCLUDE)/%, $$($(1)_HEADERS))
else
HEADERS += $$(patsubst userspace/libs/libc/%, $(BUILD_DIRECTORY_INCLUDE)/%, $$($(1)_HEADERS))
endif

$$($(1)_ARCHIVE): $$($(1)_OBJECTS)
	$$(DIRECTORY_GUARD)
	@echo [LIB$(1)] [AR] $$@
	@$(AR) $(ARFLAGS) $$@ $$^

$(BUILDROOT)/userspace/libs/lib$($(1)_NAME)/%.cpp.o: userspace/libs/lib$($(1)_NAME)/%.cpp
	$$(DIRECTORY_GUARD)
	@echo [LIB$(1)] [CXX] $$<
	@$(CXX) $(CXXFLAGS) $($(1)_CXXFLAGS) -c -o $$@ $$<

$(BUILDROOT)/userspace/libs/lib$($(1)_NAME)/%.c.o: userspace/libs/lib$($(1)_NAME)/%.c
	$$(DIRECTORY_GUARD)
	@echo [LIB$(1)] [CC] $$<
	@$(CC) $(CFLAGS) $($(1)_CFLAGS) -c -o $$@ $$<

endef

$(BUILD_DIRECTORY_INCLUDE)/%.h: userspace/libs/%.h
	$(DIRECTORY_GUARD)
	cp $< $@

$(BUILD_DIRECTORY_INCLUDE)/%.h: userspace/libs/libc/%.h
	$(DIRECTORY_GUARD)
	cp $< $@

-include userspace/libs/*/.build.mk
$(foreach lib, $(LIBS), $(eval $(call LIB_TEMPLATE,$(lib))))

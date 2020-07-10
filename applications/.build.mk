define APP_TEMPLATE =

$(1)_BINARY  = $(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/$($(1)_NAME)
$(1)_SOURCES = $$(wildcard applications/$($(1)_NAME)/*.c) \
			   $$(wildcard applications/$($(1)_NAME)/*/*.c)

$(1)_OBJECTS = $$(patsubst applications/%.c, $$(BUILD_DIRECTORY)/%.o, $$($(1)_SOURCES))

TARGETS += $$($(1)_BINARY)
OBJECTS += $$($(1)_OBJECTS)
ICONS += $$($(1)_ICONS)

$$($(1)_BINARY): $$($(1)_OBJECTS) $$(patsubst %, $$(BUILD_DIRECTORY_LIBS)/lib%.a, $$($(1)_LIBS) system) $(CRTS)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [LD] $($(1)_NAME)
	@$(CC) $(LDFLAGS) -o $$@ $$($(1)_OBJECTS) $$(patsubst %, -l%, $$($(1)_LIBS))
	@cp applications/$($(1)_NAME)/manifest.json $(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/manifest.json || true

$$(BUILD_DIRECTORY)/$$($(1)_NAME)/%.o: applications/$$($(1)_NAME)/%.c
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [CC] $$<
	@$(CC) $(CFLAGS) -c -o $$@ $$<

endef

-include applications/*/.build.mk
$(foreach app, $(APPS), $(eval $(call APP_TEMPLATE,$(app))))

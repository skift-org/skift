define APP_TEMPLATE =

$(1)_BINARY  = $(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/$($(1)_NAME)

$(1)_SOURCES = $$(wildcard userspace/apps/$($(1)_NAME)/*.cpp) \
			   $$(wildcard userspace/apps/$($(1)_NAME)/*/*.cpp) \
               $$(wildcard userspace/apps/$($(1)_NAME)/*.cppm) \
			   $$(wildcard userspace/apps/$($(1)_NAME)/*/*.cppm)

$(1)_ASSETS := $$(wildcard userspace/apps/$($(1)_NAME)/*.json) \
			   $$(wildcard userspace/apps/$($(1)_NAME)/*.png)

$(1)_ASSETS := $$(patsubst userspace/apps/$($(1)_NAME)/%, $(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/%, $$($(1)_ASSETS))

$(1)_OBJECTS = $$(patsubst userspace/apps/%, $$(BUILDROOT)/userspace/apps/%.o, $$($(1)_SOURCES))

TARGETS += $$($(1)_BINARY) $$($(1)_ASSETS)
OBJECTS += $$($(1)_OBJECTS)

$(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/%: userspace/apps/$($(1)_NAME)/%
	$$(DIRECTORY_GUARD)
	cp $$< $$@

$$($(1)_BINARY): $$($(1)_OBJECTS) $$(patsubst %, $$(BUILD_DIRECTORY_LIBS)/lib%.a, $$($(1)_LIBS) c) $(CRTS)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [LD] $($(1)_NAME)
	@$(CXX) $(LDFLAGS) -o $$@ $$($(1)_OBJECTS) $$(patsubst %, -l%, $$($(1)_LIBS))
	@if $(CONFIG_STRIP); then \
		echo [$(1)] [STRIP] $($(1)_NAME); \
		$(STRIP) $$@; \
	fi

$$(BUILDROOT)/userspace/apps/$$($(1)_NAME)/%.cpp.o: userspace/apps/$$($(1)_NAME)/%.cpp
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [CXX] $$<
	@$(CXX) $(CXXFLAGS) -c -o $$@ $$<

endef

-include userspace/apps/*/.build.mk
$(foreach app, $(APPS), $(eval $(call APP_TEMPLATE,$(app))))

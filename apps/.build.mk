define APP_TEMPLATE =

$(1)_BINARY  = $(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/$($(1)_NAME)

$(1)_SOURCES = $$(wildcard apps/$($(1)_NAME)/*.cpp) \
			   $$(wildcard apps/$($(1)_NAME)/*/*.cpp)

$(1)_ASSETS := $$(wildcard apps/$($(1)_NAME)/*.markup) \
			   $$(wildcard apps/$($(1)_NAME)/*.json) \
			   $$(wildcard apps/$($(1)_NAME)/*.png)

$(1)_ASSETS := $$(patsubst apps/$($(1)_NAME)/%, $(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/%, $$($(1)_ASSETS))

$(1)_OBJECTS = $$(patsubst apps/%.cpp, $$(CONFIG_BUILD_DIRECTORY)/apps/%.o, $$($(1)_SOURCES))

TARGETS += $$($(1)_BINARY) $$($(1)_ASSETS)
OBJECTS += $$($(1)_OBJECTS)

$(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/%: apps/$($(1)_NAME)/%
	$$(DIRECTORY_GUARD)
	cp $$< $$@

$$($(1)_BINARY): $$($(1)_OBJECTS) $$(patsubst %, $$(BUILD_DIRECTORY_LIBS)/lib%.a, $$($(1)_LIBS) system c) $(CRTS)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [LD] $($(1)_NAME)
	@$(CXX) $(LDFLAGS) -o $$@ $$($(1)_OBJECTS) $$(patsubst %, -l%, $$($(1)_LIBS)) -lsystem
	@if $(CONFIG_STRIP); then \
		echo [$(1)] [STRIP] $($(1)_NAME); \
		$(STRIP) $$@; \
	fi

$$(CONFIG_BUILD_DIRECTORY)/apps/$$($(1)_NAME)/%.o: apps/$$($(1)_NAME)/%.cpp
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [CXX] $$<
	@$(CXX) $(CXXFLAGS) -c -o $$@ $$<

endef

-include apps/*/.build.mk
$(foreach app, $(APPS), $(eval $(call APP_TEMPLATE,$(app))))

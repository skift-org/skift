define APP_TEMPLATE =

$(1)_BINARY  = $(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/$($(1)_NAME)

$(1)_SOURCES = $$(wildcard applications/$($(1)_NAME)/*.cpp) \
			   $$(wildcard applications/$($(1)_NAME)/*/*.cpp)

$(1)_ASSETS := $$(wildcard applications/$($(1)_NAME)/*.markup) \
			   $$(wildcard applications/$($(1)_NAME)/*.json) \
			   $$(wildcard applications/$($(1)_NAME)/*.png)

$(1)_ASSETS := $$(patsubst applications/$($(1)_NAME)/%, $(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/%, $$($(1)_ASSETS))

$(1)_OBJECTS = $$(patsubst applications/%.cpp, $$(BUILD_DIRECTORY)/applications/%.o, $$($(1)_SOURCES))

TARGETS += $$($(1)_BINARY) $$($(1)_ASSETS)
OBJECTS += $$($(1)_OBJECTS)
ICONS += $$($(1)_ICONS)

$(BUILD_DIRECTORY_APPS)/$($(1)_NAME)/%: applications/$($(1)_NAME)/%
	$$(DIRECTORY_GUARD)
	cp $$< $$@

$$($(1)_BINARY): $$($(1)_OBJECTS) $$(patsubst %, $$(BUILD_DIRECTORY_LIBS)/lib%.a, $$($(1)_LIBS) system) $(CRTS)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [LD] $($(1)_NAME)
	@$(CXX) $(LDFLAGS) -o $$@ $$($(1)_OBJECTS) $$(patsubst %, -l%, $$($(1)_LIBS))

$$(BUILD_DIRECTORY)/applications/$$($(1)_NAME)/%.o: applications/$$($(1)_NAME)/%.cpp
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [CXX] $$<
	@$(CXX) $(CXXFLAGS) -c -o $$@ $$<

endef

-include applications/*/.build.mk
$(foreach app, $(APPS), $(eval $(call APP_TEMPLATE,$(app))))

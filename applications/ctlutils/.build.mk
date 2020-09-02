CTLUTILS = \
	DISPLAYCTL \
	KEYBOARDCTL \
	NETCTL\
	POWERCTL \
	WALLPAPERCTL

DISPLAYCTL_LIBS =
DISPLAYCTL_NAME = displayctl

KEYBOARDCTL_LIBS =
KEYBOARDCTL_NAME = keyboardctl

NETCTL_LIBS =
NETCTL_NAME = netctl

POWERCTL_LIBS =
POWERCTL_NAME = powerctl

WALLPAPERCTL_LIBS = graphic
WALLPAPERCTL_NAME = wallpaperctl

define CTLUTIL_TEMPLATE =

$(1)_BINARY  = $(BUILD_DIRECTORY_UTILS)/$($(1)_NAME)
$(1)_SOURCE  = applications/ctlutils/$($(1)_NAME).cpp
$(1)_OBJECT  = $$(patsubst applications/ctlutils/%.cpp, $$(BUILD_DIRECTORY)/applications/ctlutils/%.o, $$($(1)_SOURCE))

TARGETS += $$($(1)_BINARY)
OBJECTS += $$($(1)_OBJECT)

$$($(1)_BINARY): $$($(1)_OBJECT) $$(patsubst %, $$(BUILD_DIRECTORY_LIBS)/lib%.a, $$($(1)_LIBS) system) $(CRTS)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [LD] $($(1)_NAME)
	@$(CXX) $(LDFLAGS) -o $$@ $$($(1)_OBJECT) $$(patsubst %, -l%, $$($(1)_LIBS))

$$($(1)_OBJECT): $$($(1)_SOURCE)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [CXX] $$<
	@$(CXX) $(CXXFLAGS) -c -o $$@ $$<

endef

$(foreach ctlutil, $(CTLUTILS), $(eval $(call CTLUTIL_TEMPLATE,$(ctlutil))))

UTILS = \
	__TESTEXEC \
	__TESTTERM \
	CAT \
	CLEAR \
	DISPLAYCTL \
	DSTART \
	ECHO \
	GREP \
	INIT \
	JSON \
	KEYBOARDCTL \
	KILL \
	LS \
	MARKUP \
	MKDIR \
	MV \
	NOW \
	OPEN \
	PANIC \
	SYSFETCH \
	TOUCH \
	UNLINK \
	UPTIME \
	WALLPAPERCTL

__TESTEXEC_NAME = __testexec
__TESTEXEC_LIBS =

__TESTTERM_NAME = __testterm
__TESTTERM_LIBS =

CAT_NAME = cat
CAT_LIBS =

CLEAR_NAME = clear
CLEAR_LIBS =

DISPLAYCTL_NAME = displayctl
DISPLAYCTL_LIBS =

DSTART_NAME = dstart
DSTART_LIBS =

ECHO_NAME = echo
ECHO_LIBS =

GREP_NAME = grep
GREP_LIBS =

INIT_NAME = init
INIT_LIBS =

JSON_NAME = json
JSON_LIBS = json

KEYBOARDCTL_NAME = keyboardctl
KEYBOARDCTL_LIBS =

KILL_NAME = kill
KILL_LIBS =

LS_NAME = ls
LS_LIBS =

MARKUP_NAME = markup
MARKUP_LIBS = markup

MKDIR_NAME = mkdir
MKDIR_LIBS =

MV_NAME = mv
MV_LIBS =

NOW_NAME = now
NOW_LIBS =

OPEN_NAME = open
OPEN_LIBS = json

PANIC_NAME = panic
PANIC_LIBS =

SYSFETCH_NAME = sysfetch
SYSFETCH_LIBS =

TOUCH_NAME = touch
TOUCH_LIBS =

UNLINK_NAME = unlink
UNLINK_LIBS =

UPTIME_NAME = uptime
UPTIME_LIBS =

WALLPAPERCTL_NAME = wallpaperctl
WALLPAPERCTL_LIBS = graphic

define UTIL_TEMPLATE =

$(1)_BINARY  = $(BUILD_DIRECTORY_UTILS)/$($(1)_NAME)
$(1)_SOURCE  = coreutils/$($(1)_NAME).cpp
$(1)_OBJECT  = $$(patsubst coreutils/%.cpp, $$(BUILD_DIRECTORY)/%.o, $$($(1)_SOURCE))

TARGETS += $$($(1)_BINARY)
OBJECTS += $$($(1)_OBJECT)

$$($(1)_BINARY): $$($(1)_OBJECT) $$(patsubst %, $$(BUILD_DIRECTORY_LIBS)/lib%.a, $$($(1)_LIBS) system) $(CRTS)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [LD] $($(1)_NAME)
	@$(CC) $(LDFLAGS) -o $$@ $$($(1)_OBJECT) $$(patsubst %, -l%, $$($(1)_LIBS))

$$($(1)_OBJECT): $$($(1)_SOURCE)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [CC] $$<
	@$(CC) $(CFLAGS) -c -o $$@ $$<

endef

$(foreach util, $(UTILS), $(eval $(call UTIL_TEMPLATE,$(util))))

UTILITIES = \
	__TESTEXEC \
	__TESTTERM \
	BASENAME \
	CAT \
	CLEAR \
	CP \
	DIRNAME \
	DISPLAYCTL \
	DSTART \
	ECHO \
	ENV \
	FALSE \
	GREP \
	HEAD \
	HEXDUMP \
	INIT \
	JSON \
	KEYBOARDCTL \
	KILL \
	LINK  \
	LS \
	MARKUP \
	MKDIR \
	MV \
	NETCTL\
	NOW \
	OPEN \
	PANIC \
	PIANO \
	PLAY \
	POWERCTL \
	PWD	\
	RMDIR \
	SETTINGSCTL \
	SYSFETCH \
	TAC \
	TOUCH \
	TRUE \
	UNAME \
	UNLINK \
	UNZIP \
	UPTIME \
	YES \
	ZIP

__TESTEXEC_LIBS =
__TESTEXEC_NAME = __testexec

__TESTTERM_LIBS =
__TESTTERM_NAME = __testterm

BASENAME_LIBS =
BASENAME_NAME = basename

CAT_LIBS =
CAT_NAME = cat

CLEAR_LIBS =
CLEAR_NAME = clear

CP_LIBS =
CP_NAME = cp

PLAY_LIBS =
PLAY_NAME = play

DSTART_LIBS =
DSTART_NAME = dstart

DIRNAME_LIBS =
DIRNAME_NAME = dirname

ECHO_LIBS =
ECHO_NAME = echo

ENV_LIBS =
ENV_NAME = env

GREP_LIBS =
GREP_NAME = grep

HEAD_LIBS =
HEAD_NAME = head

HEXDUMP_LIBS =
HEXDUMP_NAME = hexdump

INIT_LIBS =
INIT_NAME = init

JSON_LIBS =
JSON_NAME = json

KILL_LIBS =
KILL_NAME = kill

LINK_LIBS =
LINK_NAME = link

LS_LIBS =
LS_NAME = ls

MARKUP_LIBS = markup
MARKUP_NAME = markup

MKDIR_LIBS =
MKDIR_NAME = mkdir

MV_LIBS =
MV_NAME = mv

NOW_LIBS =
NOW_NAME = now

OPEN_LIBS =
OPEN_NAME = open

PANIC_LIBS =
PANIC_NAME = panic

RMDIR_LIBS =
RMDIR_NAME = rmdir

SYSFETCH_LIBS =
SYSFETCH_NAME = sysfetch

TAC_LIBS =
TAC_NAME = tac

TOUCH_LIBS =
TOUCH_NAME = touch

UNLINK_LIBS =
UNLINK_NAME = unlink

UPTIME_LIBS =
UPTIME_NAME = uptime

UNAME_LIBS =
UNAME_NAME = uname

TRUE_LIBS =
TRUE_NAME = true

FALSE_LIBS =
FALSE_NAME = false

YES_LIBS =
YES_NAME = yes

PWD_LIBS =
PWD_NAME = pwd

PIANO_LIBS = 
PIANO_NAME = piano

DISPLAYCTL_LIBS =
DISPLAYCTL_NAME = displayctl

KEYBOARDCTL_LIBS =
KEYBOARDCTL_NAME = keyboardctl

NETCTL_LIBS =
NETCTL_NAME = netctl

POWERCTL_LIBS =
POWERCTL_NAME = powerctl

SETTINGSCTL_LIBS = settings
SETTINGSCTL_NAME = settingsctl

WALLPAPERCTL_LIBS = graphic
WALLPAPERCTL_NAME = wallpaperctl

ZIP_LIBS = file
ZIP_NAME = zip

UNZIP_LIBS = file
UNZIP_NAME = unzip

define UTIL_TEMPLATE =

$(1)_BINARY  = $(BUILD_DIRECTORY_UTILITIES)/$($(1)_NAME)
$(1)_SOURCE  = apps/utilities/$($(1)_NAME).cpp
$(1)_OBJECT  = $$(patsubst apps/utilities/%.cpp, $$(CONFIG_BUILD_DIRECTORY)/apps/utilities/%.o, $$($(1)_SOURCE))

TARGETS += $$($(1)_BINARY)
OBJECTS += $$($(1)_OBJECT)

$$($(1)_BINARY): $$($(1)_OBJECT) $$(patsubst %, $$(BUILD_DIRECTORY_LIBS)/lib%.a, $$($(1)_LIBS) system) $(CRTS)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [LD] $($(1)_NAME)
	@$(CXX) $(LDFLAGS) -o $$@ $$($(1)_OBJECT) $$(patsubst %, -l%, $$($(1)_LIBS))
	@if $(CONFIG_STRIP); then \
		echo [$(1)] [STRIP] $($(1)_NAME); \
		$(STRIP) $$@; \
	fi

$$($(1)_OBJECT): $$($(1)_SOURCE)
	$$(DIRECTORY_GUARD)
	@echo [$(1)] [CXX] $$<
	@$(CXX) $(CXXFLAGS) -c -o $$@ $$<

endef

$(foreach util, $(UTILITIES), $(eval $(call UTIL_TEMPLATE,$(util))))

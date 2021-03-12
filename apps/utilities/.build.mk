UTILITIES = \
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

BASENAME_LIBS = io
BASENAME_NAME = basename

CAT_LIBS = io
CAT_NAME = cat

CLEAR_LIBS = io
CLEAR_NAME = clear

CP_LIBS = io
CP_NAME = cp

PLAY_LIBS = io
PLAY_NAME = play

DSTART_LIBS = io
DSTART_NAME = dstart

DIRNAME_LIBS = io
DIRNAME_NAME = dirname

ECHO_LIBS = io
ECHO_NAME = echo

ENV_LIBS = io
ENV_NAME = env

GREP_LIBS = io
GREP_NAME = grep

HEAD_LIBS = io
HEAD_NAME = head

HEXDUMP_LIBS = io
HEXDUMP_NAME = hexdump

INIT_LIBS = io
INIT_NAME = init

JSON_LIBS = io
JSON_NAME = json

KILL_LIBS = io
KILL_NAME = kill

LINK_LIBS = io
LINK_NAME = link

LS_LIBS = io
LS_NAME = ls

MARKUP_LIBS = markup io
MARKUP_NAME = markup

MKDIR_LIBS = io
MKDIR_NAME = mkdir

MV_LIBS = io
MV_NAME = mv

NOW_LIBS = io
NOW_NAME = now

OPEN_LIBS = io
OPEN_NAME = open

PANIC_LIBS = io
PANIC_NAME = panic

RMDIR_LIBS = io
RMDIR_NAME = rmdir

SYSFETCH_LIBS = io
SYSFETCH_NAME = sysfetch

TAC_LIBS = io
TAC_NAME = tac

TOUCH_LIBS = io
TOUCH_NAME = touch

UNLINK_LIBS = io
UNLINK_NAME = unlink

UPTIME_LIBS = io
UPTIME_NAME = uptime

UNAME_LIBS = io
UNAME_NAME = uname

TRUE_LIBS = io
TRUE_NAME = true

FALSE_LIBS = io
FALSE_NAME = false

YES_LIBS = io
YES_NAME = yes

PWD_LIBS = io
PWD_NAME = pwd

PIANO_LIBS = io
PIANO_NAME = piano

DISPLAYCTL_LIBS = io
DISPLAYCTL_NAME = displayctl

KEYBOARDCTL_LIBS = io
KEYBOARDCTL_NAME = keyboardctl

NETCTL_LIBS = io
NETCTL_NAME = netctl

POWERCTL_LIBS = io
POWERCTL_NAME = powerctl

SETTINGSCTL_LIBS = settings io
SETTINGSCTL_NAME = settingsctl

WALLPAPERCTL_LIBS = graphic io
WALLPAPERCTL_NAME = wallpaperctl

ZIP_LIBS = file io
ZIP_NAME = zip

UNZIP_LIBS = file io
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
	@$(CXX) $(LDFLAGS) -o $$@ $$($(1)_OBJECT) $$(patsubst %, -l%, $$($(1)_LIBS)) -lsystem
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

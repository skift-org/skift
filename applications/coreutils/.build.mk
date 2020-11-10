UTILS = \
	__TESTEXEC \
	__TESTTERM \
	BASENAME \
	CAT \
	CLEAR \
	CP \
	DSTART \
	DIRNAME \
	ECHO \
	ENV \
	GREP \
	HEAD \
	HEXDUMP \
	INIT \
	JSON \
	KILL \
	LS \
	MARKUP \
	MKDIR \
	RMDIR \
	MV \
	NOW \
	OPEN \
	PANIC \
	SYSFETCH \
	TAC \
	TOUCH \
	UNLINK \
	UPTIME \
	LINK  \
	UNAME \
	TRUE \
	FALSE \
	YES \
	PWD	\
	PLAY

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

define UTIL_TEMPLATE =

$(1)_BINARY  = $(BUILD_DIRECTORY_UTILS)/$($(1)_NAME)
$(1)_SOURCE  = applications/coreutils/$($(1)_NAME).cpp
$(1)_OBJECT  = $$(patsubst applications/coreutils/%.cpp, $$(BUILD_DIRECTORY)/applications/coreutils/%.o, $$($(1)_SOURCE))

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

$(foreach util, $(UTILS), $(eval $(call UTIL_TEMPLATE,$(util))))

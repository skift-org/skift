
# ---------------------------------------------------------------------------- #
#             DON'T EDIT THIS FILE, USE ENV VARIABLES or USER.MK               #
# ---------------------------------------------------------------------------- #

CONFIGS = \
	CONFIG \
	CONFIG_ARCH \
	CONGIG_COMPILER \
	CONFIG_BUILD_DIRECTORY \
	CONFIG_NOREBOOT \
	CONFIG_NOSHUTDOWN \
	CONFIG_DISPLAY \
	CONFIG_VMACHINE \
	CONFIG_KEYBOARD_LAYOUT \
	CONFIG_LOADER \
	CONFIG_LOG \
	CONFIG_LTO \
	CONFIG_MEMORY \
	CONFIG_NAME \
	CONFIG_OPTIMISATIONS \
	CONFIG_VERSION \
	CONFIG_IS_TEST \
	CONFIG_IS_RELEASE

CONFIGS_PASS_TO_COMPILER = \
	CONFIG \
	CONFIG_ARCH \
	CONFIG_TOOLCHAIN \
	CONFIG_KEYBOARD_LAYOUT \
	CONFIG_NAME \
	CONFIG_VERSION \
	CONFIG_IS_TEST \
	CONFIG_IS_RELEASE

CONFIG                ?=develop

include meta/configs/user.mk
include meta/configs/$(CONFIG).mk

# Set the target architecture.
# Possible values: x86_32/x86_64
CONFIG_ARCH           ?=x86_32

# Set the toolchaine to use
# Possible values: gnu/llvm
CONFIG_TOOLCHAIN	  ?=gnu

# Set the directory where output file will be generated.
CONFIG_BUILD_DIRECTORY?=$(shell pwd)/build

# Prevent the virtual machine to reboot (if supported).
CONFIG_NOREBOOT       ?=false

# Prevent the virtual machine to shutdown (if supported).
CONFIG_NOSHUTDOWN     ?=false

# Set the display backend of the virtual machine (if supported).
# Possible values: sdl/gtk
CONFIG_DISPLAY        ?=sdl

# Set which virtual machine to use when doing `make run`.
# Possible values: qemu/bochs/vbox
CONFIG_VMACHINE       ?=qemu

# Set the default keyboard layout.
# Possible values: (check the content of /System/Keyboard)
CONFIG_KEYBOARD_LAYOUT?=en_us

# Set the bootloader.
# Possible values: grub, limine
CONFIG_LOADER         ?=grub

# Enable/disable the logger.
CONFIG_LOG            ?=true

# Enable/disable the LTO.
CONFIG_LTO            ?=true

# Enable/disable stripping of binaries.
CONFIG_STRIP		  ?=true

# How many megabyte of memory is allocated to the virtual machine.
CONFIG_MEMORY         ?=256

# Set the name of the distribution.
CONFIG_NAME           ?=skift

# The optimisation level used by the compiler.
CONFIG_OPTIMISATIONS  ?=-O2

# The version number (usualy year.week).
CONFIG_VERSION        ?=${shell date +'%y.%W'}

# Should unit tests be run when the system start
CONFIG_IS_TEST        ?=false

# Is it a release
CONFIG_IS_RELEASE     ?=false

define BUILD_CONFIG_TEMPLATE =
-D__$(1)__=$(if $(filter $($(1)),true),1,$(if $(filter false, $($(1))),0,\""$($(1))"\"))
endef

BUILD_CONFIGS := $(foreach cfg, $(CONFIGS_PASS_TO_COMPILER), $(call BUILD_CONFIG_TEMPLATE,$(cfg)))

print-config:
	@echo $(BUILD_CONFIGS)

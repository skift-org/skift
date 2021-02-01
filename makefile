.SUFFIXES:
.DEFAULT_GOAL := all

export PATH := $(shell toolchain/use-it.sh):$(PATH)
export PATH := $(shell toolbox/use-it.sh):$(PATH)
export LC_ALL=C

ifeq (, $(shell which convert))
$(error "No ImageMagick in PATH, consider installing it")
endif

DIRECTORY_GUARD=@mkdir -p $(@D)

include configs/defaults.mk

BUILD_SYSTEM?=skift
BUILD_DISTRO?=$(CONFIG_LOADER)-$(CONFIG_ARCH)

BUILD_TARGET=$(CONFIG)-$(CONFIG_ARCH)-$(BUILD_SYSTEM)
BUILD_GITREF=$(shell git rev-parse --abbrev-ref HEAD || echo unknown)@$(shell git rev-parse --short HEAD || echo unknown)
BUILD_UNAME=$(shell uname -s -o -m -r)
DISKS_DIRECTORY=$(shell pwd)/disks

SYSROOT=$(CONFIG_BUILD_DIRECTORY)/sysroot
BOOTROOT=$(CONFIG_BUILD_DIRECTORY)/bootroot-$(CONFIG_LOADER)-$(CONFIG_ARCH)
BOOTDISK=$(DISKS_DIRECTORY)/bootdisk-$(CONFIG_LOADER)-$(CONFIG_ARCH).img

RAMDISK=$(CONFIG_BUILD_DIRECTORY)/ramdisk.tar

BUILD_DIRECTORY_LIBS=$(SYSROOT)/System/Libraries
BUILD_DIRECTORY_INCLUDE=$(SYSROOT)/System/Includes
BUILD_DIRECTORY_APPS=$(SYSROOT)/Applications
BUILD_DIRECTORY_UTILITIES=$(SYSROOT)/System/Utilities

BUILD_WARNING:= \
	-Wall \
	-Wextra  \
	-Werror \

BUILD_INCLUDE:= \
	-I. \
	-Iapps \
	-Ilibraries \
	-Ilibraries/libc

BUILD_DEFINES:= \
	-D__BUILD_ARCH__=\""$(CONFIG_ARCH)"\" \
	-D__BUILD_CONFIG__=\""$(CONFIG)"\" \
	-D__BUILD_SYSTEM__=\""$(BUILD_SYSTEM)"\" \
	-D__BUILD_TARGET__=\""$(BUILD_TARGET)"\" \
	-D__BUILD_GITREF__=\""$(BUILD_GITREF)"\" \
	-D__BUILD_UNAME__=\""$(BUILD_UNAME)"\" \
	-D__BUILD_VERSION__=\""$(CONFIG_VERSION)"\"

# --- Configs -------------------------------------------- #
CC:=i686-pc-skift-gcc
CFLAGS= \
	-std=gnu11 \
	-MD \
	$(CONFIG_OPTIMISATIONS) \
	$(BUILD_WARNING) \
	$(BUILD_INCLUDE) \
	$(BUILD_DEFINES) \
	$(BUILD_CONFIGS)

CXX:=i686-pc-skift-g++
CXXFLAGS:= \
	-std=c++20 \
	-MD \
	$(CONFIG_OPTIMISATIONS) \
	$(BUILD_WARNING) \
	$(BUILD_INCLUDE) \
	$(BUILD_DEFINES) \
	$(BUILD_CONFIGS)

LD:=i686-pc-skift-ld
LDFLAGS:=

AR:=i686-pc-skift-ar
ARFLAGS:=rcs

AS=nasm
ASFLAGS=-f elf32

STRIP:=i686-pc-skift-strip

include archs/.build.mk

include thirdparty/.build.mk
include kernel/.build.mk
include libraries/.build.mk
include apps/.build.mk
include icons/.build.mk
include distros/.build.mk

# --- Ramdisk -------------------------------------------- #

SYSROOT_CONTENT=$(shell find sysroot/ -type f)

$(RAMDISK): $(CRTS) $(TARGETS) $(HEADERS) $(SYSROOT_CONTENT)
	$(DIRECTORY_GUARD)

	@echo [TAR] $@

	@mkdir -p \
		$(SYSROOT)/Applications \
		$(SYSROOT)/Session \
		$(SYSROOT)/System \
		$(SYSROOT)/System/Utilities \
		$(SYSROOT)/Configs \
		$(SYSROOT)/Files/Fonts \
		$(SYSROOT)/Files/Icons \
		$(SYSROOT)/System/Includes \
		$(SYSROOT)/System/Libraries \
		$(SYSROOT)/Files \
		$(SYSROOT)/User \
		$(SYSROOT)/User/Applications \
		$(SYSROOT)/User/Configs \
		$(SYSROOT)/User/Desktop \
		$(SYSROOT)/User/Documents \
		$(SYSROOT)/User/Download  \
		$(SYSROOT)/User/Movies  \
		$(SYSROOT)/User/Music  \
		$(SYSROOT)/User/Pictures  \
		$(SYSROOT)/User/Public  \
		$(SYSROOT)/User/Sites

	@cp -r sysroot/* $(SYSROOT)/
	@cp license.md $(SYSROOT)/Files

	@cd $(SYSROOT); tar -cf $@ *

# --- Phony ---------------------------------------------- #

.PHONY: all
all: $(BOOTDISK)

.PHONY: run
include vms/$(CONFIG_VMACHINE).mk

.PHONY: sync
sync:
	rm $(BOOTDISK) $(RAMDISK)
	make $(BOOTDISK)

.PHONY: clean
clean:
	rm -rf $(CONFIG_BUILD_DIRECTORY)

clean-fs:
	rm -rf $(SYSROOT)

.PHONY: install-headers
install-headers: $(HEADERS)

-include $(OBJECTS:.o=.d)

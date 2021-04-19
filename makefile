.SUFFIXES:
.DEFAULT_GOAL := all

export PATH := $(shell toolchain/use-it.sh):$(PATH)
export PATH := $(shell meta/utils/use-it.sh):$(PATH)
export LC_ALL=C

DIRECTORY_GUARD=@mkdir -p $(@D)

include meta/configs/defaults.mk

BUILD_SYSTEM?=skift
BUILD_DISTRO?=$(BUILD_SYSTEM)-$(CONFIG_ARCH)-$(CONFIG_LOADER)
BUILD_TARGET?=$(BUILD_SYSTEM)-$(CONFIG_ARCH)-$(CONFIG)

BUILD_GITREF=$(shell git rev-parse --abbrev-ref HEAD || echo unknown)@$(shell git rev-parse --short HEAD || echo unknown)
BUILD_UNAME=$(shell uname -s -o -m -r)
DISKS_DIRECTORY=$(CONFIG_BUILD_DIRECTORY)/disks

BUILDROOT=$(CONFIG_BUILD_DIRECTORY)/$(BUILD_TARGET)
SYSROOT=$(BUILDROOT)/sysroot
BOOTROOT=$(DISKS_DIRECTORY)/$(BUILD_DISTRO)-$(CONFIG)
BOOTDISK=$(DISKS_DIRECTORY)/$(BUILD_DISTRO)-$(CONFIG).img
BOOTDISK_GZIP=$(BOOTDISK).gz

RAMDISK=$(BUILDROOT)/ramdisk.tar

BUILD_DIRECTORY_LIBS=$(SYSROOT)/System/Libraries
BUILD_DIRECTORY_INCLUDE=$(SYSROOT)/System/Includes
BUILD_DIRECTORY_APPS=$(SYSROOT)/Applications
BUILD_DIRECTORY_UTILITIES=$(SYSROOT)/System/Utilities

BUILD_WARNING:= \
	-Wall \
	-Wextra  \
	-Werror

CXX_WARNINGS := \
	-Wnon-virtual-dtor \
	-Woverloaded-virtual

BUILD_INCLUDE:= \
	-I. \
	-Ikernel \
	-Iuserspace/ \
	-Iuserspace/apps \
	-Iuserspace/libraries \
	-Iuserspace/libraries/libc

BUILD_DEFINES:= \
	-D__BUILD_ARCH__=\""$(CONFIG_ARCH)"\" \
	-D__BUILD_CONFIG__=\""$(CONFIG)"\" \
	-D__BUILD_SYSTEM__=\""$(BUILD_SYSTEM)"\" \
	-D__BUILD_TARGET__=\""$(BUILD_TARGET)"\" \
	-D__BUILD_GITREF__=\""$(BUILD_GITREF)"\" \
	-D__BUILD_UNAME__=\""$(BUILD_UNAME)"\" \
	-D__BUILD_VERSION__=\""$(CONFIG_VERSION)"\"

# --- Configs -------------------------------------------- #

CFLAGS= \
	-std=gnu11 \
	-MD \
	--sysroot=$(SYSROOT) \
	$(CONFIG_OPTIMISATIONS) \
	$(BUILD_WARNING) \
	$(BUILD_INCLUDE) \
	$(BUILD_DEFINES) \
	$(BUILD_CONFIGS)

CXX:=i686-pc-skift-g++
CXXFLAGS:= \
	-std=c++20 \
	-MD \
	--sysroot=$(SYSROOT) \
	$(CONFIG_OPTIMISATIONS) \
	$(BUILD_WARNING) \
	$(CXX_WARNINGS) \
	$(BUILD_INCLUDE) \
	$(BUILD_DEFINES) \
	$(BUILD_CONFIGS)

include meta/toolchains/$(CONFIG_ARCH)-$(CONFIG_TOOLCHAIN).mk
include kernel/archs/.build.mk
include kernel/kernel/.build.mk

include userspace/archs/.build.mk
include userspace/libraries/.build.mk
include userspace/apps/.build.mk
include userspace/tests/.build.mk
include userspace/utilities/.build.mk

include thirdparty/.build.mk
include meta/distros/.build.mk

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

$(BOOTDISK_GZIP): $(BOOTDISK)
	@gzip -c $(BOOTDISK) > $(BOOTDISK_GZIP)

# --- Phony ---------------------------------------------- #

.PHONY: all
all: $(BOOTDISK)

.PHONY: distro
distro: $(BOOTDISK_GZIP)

.PHONY: run
include meta/vms/$(CONFIG_VMACHINE).mk

.PHONY: sync
sync:
	rm $(BOOTDISK) $(RAMDISK)
	make $(BOOTDISK)

.PHONY: clean
clean:
	rm -rf $(BUILDROOT)

.PHONY: clean-all
clean-all:
	rm -rf $(CONFIG_BUILD_DIRECTORY)

clean-fs:
	rm -rf $(SYSROOT)

.PHONY: install-headers
install-headers: $(HEADERS)

-include $(OBJECTS:.o=.d)

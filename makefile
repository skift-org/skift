.SUFFIXES:
.DEFAULT_GOAL := all

export PATH := $(shell toolchain/use-it.sh):$(PATH)
export PATH := $(shell toolbox/use-it.sh):$(PATH)
export LC_ALL=C

ifeq (, $(shell which inkscape))
$(error "No inkscape in PATH, consider installing it")
endif

DIRECTORY_GUARD=@mkdir -p $(@D)

include configurations/defaults.mk

BUILD_ARCH?=x86_32
BUILD_CONFIG?=debug
BUILD_SYSTEM?=skift
BUILD_LOADER?=grub
BUILD_DISTRO?=$(BUILD_LOADER)-$(BUILD_ARCH)

BUILD_TARGET=$(BUILD_CONFIG)-$(BUILD_ARCH)-$(BUILD_SYSTEM)
BUILD_GITREF=$(shell git rev-parse --abbrev-ref HEAD || echo unknown)@$(shell git rev-parse --short HEAD || echo unknown)
BUILD_UNAME=$(shell uname -s -o -m -r)
BUILD_DIRECTORY=$(shell pwd)/build
ASSETS_DIRECTORY=$(shell pwd)/disks

SYSROOT=$(BUILD_DIRECTORY)/sysroot
BOOTROOT=$(BUILD_DIRECTORY)/bootroot-$(BUILD_LOADER)-$(BUILD_ARCH)
BOOTDISK=$(ASSETS_DIRECTORY)/bootdisk-$(BUILD_LOADER)-$(BUILD_ARCH).img

RAMDISK=$(BUILD_DIRECTORY)/ramdisk.tar

BUILD_DIRECTORY_LIBS=$(SYSROOT)/System/Libraries
BUILD_DIRECTORY_INCLUDE=$(SYSROOT)/System/Includes
BUILD_DIRECTORY_APPS=$(SYSROOT)/Applications
BUILD_DIRECTORY_UTILS=$(SYSROOT)/System/Binaries

BUILD_OPTIMISATIONS?=-O2

BUILD_WARNING:= \
	-Wall \
	-Wextra  \
	-Werror \

BUILD_INCLUDE:= \
	-I. \
	-Iapplications \
	-Ilibraries \
	-Ilibraries/libc

BUILD_DEFINES:= \
	-D__BUILD_ARCH__=\""$(BUILD_ARCH)"\" \
	-D__BUILD_CONFIG__=\""$(BUILD_CONFIG)"\" \
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
	$(BUILD_OPTIMISATIONS) \
	$(BUILD_WARNING) \
	$(BUILD_INCLUDE) \
	$(BUILD_DEFINES) \
	$(BUILD_CONFIG)

CXX:=i686-pc-skift-g++
CXXFLAGS:= \
	-std=c++20 \
	-MD \
	$(BUILD_OPTIMISATIONS) \
	$(BUILD_WARNING) \
	$(BUILD_INCLUDE) \
	$(BUILD_DEFINES)

LD:=i686-pc-skift-ld
LDFLAGS:=

AR:=i686-pc-skift-ar
ARFLAGS:=rcs

AS=nasm
ASFLAGS=-f elf32

include architectures/.build.mk

include thirdparty/.build.mk
include protocols/.build.mk
include kernel/.build.mk
include libraries/.build.mk
include applications/.build.mk
include icons/.build.mk
include distributions/.build.mk

# --- Ramdisk -------------------------------------------- #

SYSROOT_CONTENT=$(shell find sysroot/ -type f)

$(RAMDISK): $(CRTS) $(TARGETS) $(HEADERS) $(SYSROOT_CONTENT)
	$(DIRECTORY_GUARD)

	@echo [TAR] $@

	@mkdir -p \
		$(SYSROOT)/Applications \
		$(SYSROOT)/Session \
		$(SYSROOT)/System \
		$(SYSROOT)/System/Binaries \
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
run: run-qemu


QEMU=qemu-system-x86_64
QEMU_FLAGS=-m $(CONFIG_MEMORY)M \
		  -serial stdio \
		  -rtc base=localtime

QEMU_DISK?=-cdrom $(BOOTDISK)

QEMU_FLAGS_VIRTIO=-device virtio-rng-pci \
				 -device virtio-serial \
				 -nic user,model=virtio-net-pci \
				 -vga virtio

.PHONY: run-qemu
run-qemu: $(BOOTDISK)
	@echo [QEMU] $^
	@$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS) $(QEMU_EXTRA) -enable-kvm -device ac97 || \
	$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS) $(QEMU_EXTRA) -enable-kvm -soundhw ac97 || \
	$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS) $(QEMU_EXTRA) -device ac97 || \
	$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS) $(QEMU_EXTRA) -soundhw ac97

run-qemu-no-kvm: $(BOOTDISK)
	$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS) $(QEMU_EXTRA) -device ac97 || \
		$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS) $(QEMU_EXTRA) -soundhw ac97

run-qemu-virtio: $(BOOTDISK)
	@echo [QEMU] $^
	@$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS_VIRTIO) $(QEMU_EXTRA) -enable-kvm -device ac97 || \
	$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS_VIRTIO) $(QEMU_EXTRA) -enable-kvm -soundhw ac97 || \
	$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS_VIRTIO) $(QEMU_EXTRA) -device ac97 || \
	$(QEMU) $(QEMU_DISK) $(QEMU_FLAGS_VIRTIO) $(QEMU_EXTRA) -soundhw ac97

.PHONY: run-bochs
run-bochs: $(BOOTDISK)
	bochs -q -rc .bochsini

.PHONY: run-vbox
run-vbox: $(BOOTDISK)
	@VBoxManage unregistervm --delete "skiftOS-dev" || echo "Look like it's the fist time you are running this command, this is fine"
	@VBoxManage createvm \
		--name skiftOS-dev \
		--ostype Other \
		--register \
		--basefolder $(shell pwd)/vm

	@VBoxManage modifyvm \
		skiftOS-dev \
		--memory $(CONFIG_MEMORY) \
		--uart1 0x3F8 4 \
		--uartmode1 tcpserver 1234

	@VBoxManage storagectl \
		skiftOS-dev \
		--name IDE \
		--add ide \

	@VBoxManage storageattach \
		skiftOS-dev \
		--storagectl IDE \
		--port 0 \
		--device 0 \
		--type dvddrive \
		--medium $(BOOTDISK)

ifeq ($(VBOX_DISPLAY),sdl)
	@vboxsdl --startvm skiftOS-dev &
	@sleep 1
else
	@VBoxManage startvm skiftOS-dev --type gui
endif
	nc localhost 1234


.PHONY: sync
sync:
	rm $(BOOTDISK) $(RAMDISK)
	make $(BOOTDISK)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIRECTORY)

clean-fs:
	rm -rf $(SYSROOT)

.PHONY: install-headers
install-headers: $(HEADERS)

-include $(OBJECTS:.o=.d)

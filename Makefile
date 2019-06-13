export REPOROOT = $(shell pwd)
export PATH = $(shell toolchain/use-it!.sh)
export BUILDROOT = $(REPOROOT)/build
export SYSROOT = $(BUILDROOT)/sysroot
export BOOTROOT = $(BUILDROOT)/bootroot

export CC=i686-pc-skift-gcc
export CFLAGS=-std=gnu11 -O2 -Wall -Wextra -Werror -D__COMMIT__=\"$(shell git log --pretty=format:'%h' -n 1)\"

export LD=i686-pc-skift-ld
export LDFLAGS=-flto

export AS=nasm
export ASFLAGS=-f elf32

export AR=i686-pc-skift-ar

TARGETS=libraries userspace kernel shell usertests

TARGETS_BUILD=${TARGETS:=.build}
TARGETS_INSTALL=${TARGETS:=.install}
TARGETS_CLEAN=${TARGETS:=.clean}

INCLUDES=$(shell find -type d -name include ! -path "./toolchain/*" ! -path "./build/*")

all: build/bootdisk.iso

run: all
	qemu-system-i386 -cdrom build/bootdisk.iso  -m 256M -enable-kvm -serial mon:stdio

clean: $(TARGETS_CLEAN)
	rm -rf $(SYSROOT)
	rm -rf $(BOOTROOT)
	rm -rf build/ramdisk.tar
	rm -rf build/bootdisk.iso

list:
	@echo $(TARGETS)
	@echo $(TARGETS_INSTALL)
	@echo $(TARGETS_CLEAN)

sync:
	cp -a sysroot/* $(SYSROOT)
	cp -a $(INCLUDES) $(SYSROOT)/lib/include/

%.install: %
	make -C $^ install

%.clean: %
	make -C $^ clean

.PHONY: all run clean sync

$(SYSROOT):
	mkdir -p $(SYSROOT)
	mkdir -p $(SYSROOT)/bin
	mkdir -p $(SYSROOT)/dev
	mkdir -p $(SYSROOT)/lib
	mkdir -p $(SYSROOT)/me
	mkdir -p $(SYSROOT)/me/share
	mkdir -p $(SYSROOT)/me/anon
	mkdir -p $(SYSROOT)/me/root
	cp -a sysroot/* $(SYSROOT)
	cp -ua $(INCLUDES) $(SYSROOT)/lib/

$(BOOTROOT):
	mkdir -p $(BOOTROOT)
	mkdir -p $(BOOTROOT)/boot
	mkdir -p $(BOOTROOT)/boot/grub
	cp grub.cfg $(BOOTROOT)/boot/grub/

build/ramdisk.tar: $(SYSROOT) $(TARGETS_INSTALL)
	cd $(SYSROOT); tar -cf ../../$@ *
	
build/bootdisk.iso: $(BOOTROOT) build/ramdisk.tar
	cp build/ramdisk.tar $(BOOTROOT)/boot/
	grub-mkrescue -o $@ $(BOOTROOT)

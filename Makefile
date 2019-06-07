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

INCLUDES=$(shell find -type d -name include ! -path "./toolchain/*" ! -path "./build/*")

all: build/bootdisk.iso

run: all
	qemu-system-i386 -cdrom build/bootdisk.iso  -m 256M -enable-kvm -serial mon:stdio

clean:
	rm -rf $(SYSROOT)
	rm -rf $(BOOTROOT)
	rm -rf build/ramdisk.tar
	rm -rf build/bootdisk.iso

	make -C libraries clean
	make -C coreutils clean
	make -C drivers clean
	make -C kernel clean
	make -C shell clean
	make -C tests clean
	make -C tests applications

libraries: $(SYSROOT)
	make -C libraries install

coreutils: $(SYSROOT) libraries
	make -C coreutils install

applications: $(SYSROOT) libraries
	make -C applications install

drivers: $(SYSROOT) libraries
	make -C drivers install

tests: $(SYSROOT) libraries
	make -C tests install

kernel: $(BOOTROOT) libraries
	make -C kernel install

shell: $(SYSROOT) libraries
	make -C shell install

sync:
	cp -a $(INCLUDES) $(SYSROOT)/lib/include/

.PHONY: all clean libraries coreutils kernel shell tests sync applications

$(SYSROOT):
	mkdir -p $(SYSROOT)
	mkdir -p $(SYSROOT)/bin
	mkdir -p $(SYSROOT)/dev
	mkdir -p $(SYSROOT)/lib
	mkdir -p $(SYSROOT)/me
	mkdir -p $(SYSROOT)/me/share
	mkdir -p $(SYSROOT)/me/anon
	mkdir -p $(SYSROOT)/me/root
	cp -ua $(INCLUDES) $(SYSROOT)/lib/

$(BOOTROOT):
	mkdir -p $(BOOTROOT)
	mkdir -p $(BOOTROOT)/boot
	mkdir -p $(BOOTROOT)/boot/grub
	cp grub.cfg $(BOOTROOT)/boot/grub/

build/ramdisk.tar: $(SYSROOT) shell kernel drivers coreutils applications tests
	cd $(SYSROOT); tar -cf ../../$@ *
	
build/bootdisk.iso: $(BOOTROOT) build/ramdisk.tar
	cp build/ramdisk.tar $(BOOTROOT)/boot/
	grub-mkrescue -o $@ $(BOOTROOT)

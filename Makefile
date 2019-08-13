.SECONDARY:

PATH := $(shell toolchain/use-it!.sh):$(PATH)
PATH := $(shell toolbox/use-it!.sh):$(PATH)

TARGET_TRIPLET=i686-pc-skift

CC=$(TARGET_TRIPLET)-gcc
CFLAGS_WARN=-Wall -Wextra -Werror
CFLAGS=-std=gnu11 -O2 $(CFLAGS_WARN) -Ilibraries -D__COMMIT__=\"$(shell git log --pretty=format:'%h' -n 1)\"

AS=nasm
ASFLAGS=-f elf32

AR=$(TARGET_TRIPLET)-ar
ARFLAGS=rcs

LD=$(TARGET_TRIPLET)-ld
LDFLAGS=-flto

# --- Objects ---------------------------------------------------------------- #

REPOROOT=$(shell pwd)
BUILDROOT=$(REPOROOT)/build

SYSROOT=$(BUILDROOT)/sysroot

BOOTROOT=$(BUILDROOT)/bootroot

INCLUDES=$(patsubst libraries/%.h,$(SYSROOT)/lib/include/%.h,$(shell find libraries/ -name *.h))

LIBCONSOLE=$(SYSROOT)/lib/libconsole.a
LIBCONSOLE_SRC=$(wildcard libraries/libconsole/*.c)
LIBCONSOLE_OBJ=$(patsubst %.c,%.o,$(LIBCONSOLE_SRC))

LIBDEVICE=$(SYSROOT)/lib/libdevice.a
LIBDEVICE_SRC=$(wildcard libraries/libdevice/*.c)
LIBCONSOLE_OBJ=$(patsubst %.c,%.o,$(LIBCONSOLE_SRC))

LIBFILE=$(SYSROOT)/lib/libfile.a
LIBFILE_SRC=$(wildcard libraries/libfile/*.c)
LIBFILE_OBJ=$(patsubst %.c,%.o,$(LIBFILE_SRC))

LIBFORM=$(SYSROOT)/lib/libform.a
LIBFORM_SRC=$(wildcard libraries/libform/*.c)
LIBFORM_OBJ=$(patsubst %.c,%.o,$(LIBFORM_SRC))

LIBGRAPHIC=$(SYSROOT)/lib/libgraphic.a
LIBGRAPHIC_SRC=$(wildcard libraries/libgraphic/*.c)
LIBGRAPHIC_OBJ=$(patsubst %.c,%.o,$(LIBGRAPHIC_SRC))

LIBKERNEL=$(SYSROOT)/lib/libkernel.a
LIBKERNEL_SRC=$(wildcard libraries/libkernel/*.c)
LIBKERNEL_OBJ=$(patsubst %.c,%.o,$(LIBKERNEL_SRC))

LIBMATH=$(SYSROOT)/lib/libmath.a
LIBMATH_SRC=$(wildcard libraries/libmath/*.c)
LIBMATH_OBJ=$(patsubst %.c,%.o,$(LIBMATH_SRC))

LIBSYSTEM=$(SYSROOT)/lib/libsystem.a
LIBSYSTEM_SRC=$(wildcard libraries/libsystem/*.c) $(wildcard libraries/libsystem/plugs/*.c)
LIBSYSTEM_OBJ=$(patsubst %.c,%.o,$(LIBSYSTEM_SRC))

CRTS=$(SYSROOT)/lib/crt0.o \
	 $(SYSROOT)/lib/crti.o \
	 $(SYSROOT)/lib/crtn.o

LIBRARIES=$(LIBCONSOLE) \
		  $(LIBDEVICE) \
		  $(LIBFILE)  \
		  $(LIBFORM) \
		  $(LIBGRAPHIC) \
		  $(LIBKERNEL) \
		  $(LIBMATH) \
		  $(LIBSYSTEM)

USERSPACE=$(SYSROOT)/bin/__democolors \
		  $(SYSROOT)/bin/__demofonts \
		  $(SYSROOT)/bin/__demolines \
		  $(SYSROOT)/bin/__testapp \
		  $(SYSROOT)/bin/__testargs \
		  $(SYSROOT)/bin/__testexec \
		  $(SYSROOT)/bin/__testgfx \
		  $(SYSROOT)/bin/__testterm \
		  $(SYSROOT)/bin/cat \
		  $(SYSROOT)/bin/clear \
		  $(SYSROOT)/bin/dstart \
		  $(SYSROOT)/bin/echo \
		  $(SYSROOT)/bin/gfxctl \
		  $(SYSROOT)/bin/grep \
		  $(SYSROOT)/bin/init \
		  $(SYSROOT)/bin/kill \
		  $(SYSROOT)/bin/ls \
		  $(SYSROOT)/bin/lsproc \
		  $(SYSROOT)/bin/mkdir \
		  $(SYSROOT)/bin/mv \
		  $(SYSROOT)/bin/panic \
		  $(SYSROOT)/bin/sh \
		  $(SYSROOT)/bin/sysfetch \
		  $(SYSROOT)/bin/term \
		  $(SYSROOT)/bin/touch \
		  $(SYSROOT)/bin/unlink \
		  $(SYSROOT)/bin/wm
			
KERNEL=$(BUILDROOT)/kernel.bin
KERNEL_CSOURCES=$(wildcard kernel/*.c) $(wildcard kernel/*/*.c) $(wildcard libraries/libsystem/*.c) $(wildcard libraries/libfile/*.c)
KERNEL_SSOURCES=$(wildcard kernel/*.s) $(wildcard kernel/*/*.s)
KERNEL_OBJECT= ${KERNEL_CSOURCES:.c=.kernel.o} ${KERNEL_SSOURCES:.s=.kernel.o}

RAMDISK=$(BUILDROOT)/ramdisk.tar

BOOTDISK=$(BUILDROOT)/bootdisk.iso

# --- Tagets ----------------------------------------------------------------- #

all: $(BOOTDISK)

clean:
	rm -rf $(BUILDROOT)
	find userspace/ -name "*.o" -delete
	find libraries/ -name "*.o" -delete
	find kernel/ -name "*.o" -delete

run: $(BOOTDISK)
	qemu-system-i386 -cdrom $^ -m 256M -serial mon:stdio -enable-kvm  || \
	qemu-system-i386 -cdrom $^ -m 256M -serial mon:stdio

list:
	@echo ""
	@echo "$(LIBRARIES)"
	@echo ""
	@echo "$(USERSPACE)"
	@echo ""
	@echo "$(RAMDISK)"
	@echo ""
	@echo "$(BOOTDISK)"
	@echo ""
	@echo "$(INCLUDES)"
	@echo ""
	@echo "$(RESSOURCES)"

# --- Libaries --------------------------------------------------------------- #

$(SYSROOT)/lib/crt0.o: libraries/crt0.s
	mkdir -p $(SYSROOT)/lib/
	$(AS) $(ASFLAGS) -o $@ $^

$(SYSROOT)/lib/crti.o: libraries/crti.s
	mkdir -p $(SYSROOT)/lib/
	$(AS) $(ASFLAGS) -o $@ $^

$(SYSROOT)/lib/crtn.o: libraries/crtn.s
	mkdir -p $(SYSROOT)/lib/
	$(AS) $(ASFLAGS) -o $@ $^

$(LIBCONSOLE): $(LIBCONSOLE_OBJ)
	mkdir -p $(SYSROOT)/lib/
	$(AR) $(ARFLAGS) $@ $^

$(LIBDEVICE): $(LIBCONSOLE_OBJ)
	mkdir -p $(SYSROOT)/lib/
	$(AR) $(ARFLAGS) $@ $^

$(LIBFILE): $(LIBFILE_OBJ)
	mkdir -p $(SYSROOT)/lib/
	$(AR) $(ARFLAGS) $@ $^

$(LIBFORM): $(LIBFORM_OBJ)
	mkdir -p $(SYSROOT)/lib/
	$(AR) $(ARFLAGS) $@ $^

$(LIBGRAPHIC): $(LIBGRAPHIC_OBJ)
	mkdir -p $(SYSROOT)/lib/
	$(AR) $(ARFLAGS) $@ $^

$(LIBKERNEL): $(LIBKERNEL_OBJ)
	mkdir -p $(SYSROOT)/lib/
	$(AR) $(ARFLAGS) $@ $^

$(LIBMATH): $(LIBMATH_OBJ)
	mkdir -p $(SYSROOT)/lib/
	$(AR) $(ARFLAGS) $@ $^

$(LIBSYSTEM): $(LIBSYSTEM_OBJ)
	mkdir -p $(SYSROOT)/lib/
	$(AR) $(ARFLAGS) $@ $^

# --- Userspace -------------------------------------------------------------- #

$(SYSROOT)/bin/__democolors: userspace/__democolors.c $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

$(SYSROOT)/bin/__demofonts: userspace/__demofonts.c $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

$(SYSROOT)/bin/__demolines: userspace/__demolines.c $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

$(SYSROOT)/bin/__testapp: userspace/__testapp.c $(LIBSYSTEM) $(LIBFORM) $(LIBGRAPHIC) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@ -lform -lgraphic

$(SYSROOT)/bin/__testargs: userspace/__testargs.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/__testexec: userspace/__testexec.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/__testgfx: userspace/__testgfx.c $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

$(SYSROOT)/bin/__testterm: userspace/__testterm.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/cat: userspace/cat.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/clear: userspace/clear.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/dstart: userspace/dstart.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/echo: userspace/echo.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/gfxctl: userspace/gfxctl.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/grep: userspace/grep.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/init: userspace/init.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/kill: userspace/kill.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/ls: userspace/ls.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/lsproc: userspace/lsproc.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/mkdir: userspace/mkdir.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/mv: userspace/mv.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/panic: userspace/panic.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/sh: userspace/sh.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/sysfetch: userspace/sysfetch.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/term: userspace/term.c $(LIBSYSTEM) $(LIBCONSOLE) $(LIBGRAPHIC) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@ -lconsole -lgraphic

$(SYSROOT)/bin/touch: userspace/touch.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/unlink: userspace/unlink.c $(LIBSYSTEM) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@

$(SYSROOT)/bin/wm: userspace/wm.c $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	mkdir -p $(SYSROOT)/bin
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

# --- Kernel ----------------------------------------------------------------- #

%.kernel.o: %.c
	$(CC) $(CFLAGS) -ffreestanding -nostdlib -Ikernel/ -c -o $@ $^

%.kernel.o: %.s
	$(AS) $(ASFLAGS) $^ -o $@

$(KERNEL): $(KERNEL_OBJECT)
	mkdir -p $(BUILDROOT)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(KERNEL_OBJECT)

# --- Ressources ------------------------------------------------------------- #

WALLPAPERS = ${patsubst ressources/%,$(SYSROOT)/res/%,${wildcard ressources/wallpaper/*.png}}
FONTS_GLYPHS = ${patsubst ressources/%,$(SYSROOT)/res/%,${wildcard ressources/font/*.glyph}}
FONTS_PNGS = ${patsubst ressources/%,$(SYSROOT)/res/%,${wildcard ressources/font/*.png}}
RESSOURCES = $(WALLPAPERS) $(FONTS_GLYPHS) $(FONTS_PNGS)

$(SYSROOT)/res/font/%.glyph: ressources/font/%.json
	mkdir -p $(SYSROOT)/res/font
	fontcompiler.py $^ $@

$(SYSROOT)/res/font/%.png: ressources/font/%.png
	mkdir -p $(SYSROOT)/res/font
	cp $^ $@

$(SYSROOT)/res/wallpaper/%.png: ressources/wallpaper/%.png
	mkdir -p $(SYSROOT)/res/wallpaper
	cp $^ $@

# --- Ramdisk ---------------------------------------------------------------- #

$(SYSROOT)/lib/include/%.h: libraries/%.h
	mkdir -p $(SYSROOT)/lib/include
	cp $^ $(SYSROOT)/lib/include

$(SYSROOT):
	mkdir -p $(SYSROOT)
	mkdir -p $(SYSROOT)/bin \
			 $(SYSROOT)/dev \
			 $(SYSROOT)/etc \
			 $(SYSROOT)/lib \
			 $(SYSROOT)/res \
			 $(SYSROOT)/run \
			 $(SYSROOT)/usr

$(RAMDISK): $(SYSROOT) $(USERSPACE) $(RESSOURCES)
	cd $(SYSROOT); tar -cf $@ *

# --- Bootdisk --------------------------------------------------------------- #

$(BOOTROOT): grub.cfg $(KERNEL) $(RAMDISK)
	mkdir -p $(BOOTROOT)/boot/grub
	cp grub.cfg $(BOOTROOT)/boot/grub/
	cp $(KERNEL) $(BOOTROOT)/boot
	cp $(RAMDISK) $(BOOTROOT)/boot

$(BOOTDISK): $(BOOTROOT)
	grub-mkrescue -o $@ $(BOOTROOT) || grub2-mkrescue -o $@ $(BOOTROOT)

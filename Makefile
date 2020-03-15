.SECONDARY:

PATH := $(shell toolchain/use-it!.sh):$(PATH)
PATH := $(shell toolbox/use-it!.sh):$(PATH)

DIRECTORY_GUARD=@mkdir -p $(@D)

TARGET_TRIPLET=i686-pc-skift

CC=$(TARGET_TRIPLET)-gcc

COPT_FLAGS=-O2 -pipe

CDIALECT_FLAGS=-std=gnu11

CWARN_FLAGS=-Wall \
		    -Wextra \
			-Werror


CFLAGS=$(CDIALECT_FLAGS) \
	   $(COPT_FLAGS) \
	   $(CWARN_FLAGS) \
	   -I. -Iapplication -Ilibraries -Ilibraries/libcompat \
	   -D__COMMIT__=\"$(shell git log --pretty=format:'%h' -n 1)\"

QEMU=qemu-system-x86_64

AS=nasm
ASFLAGS=-f elf32

AR=$(TARGET_TRIPLET)-ar
ARFLAGS=rcs

LD=$(TARGET_TRIPLET)-ld
LDFLAGS=-flto

# --- Objects ---------------------------------------------------------------- #

BUILD_DIRECTORY=$(shell pwd)/build
ROOT_DIRECTORY=$(BUILD_DIRECTORY)/sysroot
BOOT_DIRECTORY=$(BUILD_DIRECTORY)/bootroot

INCLUDES=$(patsubst libraries/%.h,$(ROOT_DIRECTORY)/lib/include/%.h,$(shell find libraries/ -path libraries/libcompat -prune -o -name *.h))
INCLUDES+=$(patsubst libraries/libcompat/%.h,$(ROOT_DIRECTORY)/lib/include/%.h,$(shell find libraries/libcompat -name *.h))

LIBCONSOLE=$(ROOT_DIRECTORY)/lib/libterminal.a
LIBCONSOLE_SRC=$(wildcard libraries/libterminal/*.c)
LIBCONSOLE_OBJ=$(patsubst %.c,%.o,$(LIBCONSOLE_SRC))

LIBDEVICE=$(ROOT_DIRECTORY)/lib/libdevice.a
LIBDEVICE_SRC=$(wildcard libraries/libdevice/*.c)
LIBCONSOLE_OBJ=$(patsubst %.c,%.o,$(LIBCONSOLE_SRC))

LIBFILE=$(ROOT_DIRECTORY)/lib/libfile.a
LIBFILE_SRC=$(wildcard libraries/libfile/*.c)
LIBFILE_OBJ=$(patsubst %.c,%.o,$(LIBFILE_SRC))

LIBWIDGET=$(ROOT_DIRECTORY)/lib/libwidget.a
LIBWIDGET_SRC=$(wildcard libraries/libwidget/*.c) $(wildcard libraries/libwidget/**/*.c)
LIBWIDGET_OBJ=$(patsubst %.c,%.o,$(LIBWIDGET_SRC))

LIBGRAPHIC=$(ROOT_DIRECTORY)/lib/libgraphic.a
LIBGRAPHIC_SRC=$(wildcard libraries/libgraphic/*.c)
LIBGRAPHIC_OBJ=$(patsubst %.c,%.o,$(LIBGRAPHIC_SRC))

LIBKERNEL=$(ROOT_DIRECTORY)/lib/libkernel.a
LIBKERNEL_SRC=$(wildcard libraries/libkernel/*.c)
LIBKERNEL_OBJ=$(patsubst %.c,%.o,$(LIBKERNEL_SRC))

LIBMATH=$(ROOT_DIRECTORY)/lib/libmath.a
LIBMATH_SRC=$(wildcard libraries/libmath/*.c)
LIBMATH_OBJ=$(patsubst %.c,%.o,$(LIBMATH_SRC))

LIBCOMPAT=$(ROOT_DIRECTORY)/lib/libcompat.a
LIBCOMPAT_SRC=$(wildcard libraries/libcompat/*.c)
LIBCOMPAT_OBJ=$(patsubst %.c,%.o,$(LIBCOMPAT_SRC))

LIBSYSTEM=$(ROOT_DIRECTORY)/lib/libsystem.a
LIBSYSTEM_SRC=$(wildcard libraries/libsystem/*.c) \
			  $(wildcard libraries/libsystem/plugs/*.c) \
			  $(wildcard libraries/libsystem/unicode/*.c) \
			  $(wildcard libraries/libsystem/io/*.c) \
			  $(wildcard libraries/libsystem/process/*.c) \
			  $(wildcard libraries/libsystem/eventloop/*.c) \
			  $(wildcard libraries/libsystem/readline/*.c) \
			  $(wildcard libraries/libsystem/utils/*.c)

LIBSYSTEM_OBJ=$(patsubst %.c,%.o,$(LIBSYSTEM_SRC))

CRTS=$(ROOT_DIRECTORY)/lib/crt0.o \
	 $(ROOT_DIRECTORY)/lib/crti.o \
	 $(ROOT_DIRECTORY)/lib/crtn.o

LIBRARIES=$(LIBCONSOLE) \
		  $(LIBDEVICE) \
		  $(LIBFILE)  \
		  $(LIBWIDGET) \
		  $(LIBGRAPHIC) \
		  $(LIBKERNEL) \
		  $(LIBMATH) \
		  $(LIBCOMPAT) \
		  $(LIBSYSTEM)

APPLICATION=$(ROOT_DIRECTORY)/bin/Terminal \
			$(ROOT_DIRECTORY)/bin/Shell \
			$(ROOT_DIRECTORY)/bin/Compositor

USERSPACE=$(ROOT_DIRECTORY)/bin/__democolors \
		  $(ROOT_DIRECTORY)/bin/__demolines \
		  $(ROOT_DIRECTORY)/bin/__test3d \
		  $(ROOT_DIRECTORY)/bin/__testapp \
		  $(ROOT_DIRECTORY)/bin/__testargs \
		  $(ROOT_DIRECTORY)/bin/__testexec \
		  $(ROOT_DIRECTORY)/bin/__testgfx \
		  $(ROOT_DIRECTORY)/bin/__testposix \
		  $(ROOT_DIRECTORY)/bin/__testterm \
		  $(ROOT_DIRECTORY)/bin/cat \
		  $(ROOT_DIRECTORY)/bin/clear \
		  $(ROOT_DIRECTORY)/bin/dstart \
		  $(ROOT_DIRECTORY)/bin/echo \
		  $(ROOT_DIRECTORY)/bin/displayctl \
		  $(ROOT_DIRECTORY)/bin/grep \
		  $(ROOT_DIRECTORY)/bin/init \
		  $(ROOT_DIRECTORY)/bin/kill \
		  $(ROOT_DIRECTORY)/bin/keyboardctl \
		  $(ROOT_DIRECTORY)/bin/ls \
		  $(ROOT_DIRECTORY)/bin/lsproc \
		  $(ROOT_DIRECTORY)/bin/mkdir \
		  $(ROOT_DIRECTORY)/bin/mv \
		  $(ROOT_DIRECTORY)/bin/now \
		  $(ROOT_DIRECTORY)/bin/panic \
		  $(ROOT_DIRECTORY)/bin/sysfetch \
		  $(ROOT_DIRECTORY)/bin/touch \
		  $(ROOT_DIRECTORY)/bin/unlink

KERNEL=$(BUILD_DIRECTORY)/kernel.bin
KERNEL_CSOURCES=$(wildcard kernel/*.c) \
				$(wildcard kernel/*/*.c) \
				$(wildcard libraries/libfile/*.c) \
				$(wildcard libraries/libsystem/*.c) \
				$(wildcard libraries/libsystem/io/*.c) \
				$(wildcard libraries/libsystem/unicode/*.c) \
				$(wildcard libraries/libsystem/process/*.c) \
				$(wildcard libraries/libsystem/utils/*.c)


KERNEL_SSOURCES=$(wildcard kernel/*.s) $(wildcard kernel/*/*.s)
KERNEL_OBJECT= ${KERNEL_CSOURCES:.c=.c.kernel.o} ${KERNEL_SSOURCES:.s=.s.kernel.o}

RAMDISK=$(BUILD_DIRECTORY)/ramdisk.tar

BOOTDISK=$(BUILD_DIRECTORY)/bootdisk.iso

# --- Tagets ----------------------------------------------------------------- #

all: $(BOOTDISK)

clean:
	rm -rf $(BUILD_DIRECTORY)
	find userspace/ -name "*.o" -delete
	find libraries/ -name "*.o" -delete
	find kernel/ -name "*.o" -delete

run: $(BOOTDISK)
	$(QEMU) -cdrom $^ -m 256M -serial mon:stdio -enable-kvm  || \
	$(QEMU) -cdrom $^ -m 256M -serial mon:stdio

run-headless: $(BOOTDISK)
	$(QEMU) -cdrom $^ -m 256M -serial mon:stdio -nographic -enable-kvm  || \
	$(QEMU) -cdrom $^ -m 256M -serial mon:stdio -nographic

debug: $(BOOTDISK)
	$(QEMU) -s -S -cdrom $^ -m 256M -serial mon:stdio -enable-kvm  || \
	$(QEMU) -s -S -cdrom $^ -m 256M -serial mon:stdio &

	gdb -x gdbinit

list:
	@echo "$(KERNEL_SSOURCES)"

# --- Libaries --------------------------------------------------------------- #

$(ROOT_DIRECTORY)/lib/crt0.o: libraries/crt0.s
	$(DIRECTORY_GUARD)
	$(AS) $(ASFLAGS) -o $@ $^

$(ROOT_DIRECTORY)/lib/crti.o: libraries/crti.s
	$(DIRECTORY_GUARD)
	$(AS) $(ASFLAGS) -o $@ $^

$(ROOT_DIRECTORY)/lib/crtn.o: libraries/crtn.s
	$(DIRECTORY_GUARD)
	$(AS) $(ASFLAGS) -o $@ $^

$(LIBCONSOLE): $(LIBCONSOLE_OBJ)
	$(DIRECTORY_GUARD)
	$(AR) $(ARFLAGS) $@ $^

$(LIBDEVICE): $(LIBCONSOLE_OBJ)
	$(DIRECTORY_GUARD)
	$(AR) $(ARFLAGS) $@ $^

$(LIBFILE): $(LIBFILE_OBJ)
	$(DIRECTORY_GUARD)
	$(AR) $(ARFLAGS) $@ $^

$(LIBWIDGET): $(LIBWIDGET_OBJ)
	$(DIRECTORY_GUARD)
	$(AR) $(ARFLAGS) $@ $^

$(LIBGRAPHIC): $(LIBGRAPHIC_OBJ)
	$(DIRECTORY_GUARD)
	$(AR) $(ARFLAGS) $@ $^

$(LIBKERNEL): $(LIBKERNEL_OBJ)
	$(DIRECTORY_GUARD)
	$(AR) $(ARFLAGS) $@ $^

$(LIBMATH): $(LIBMATH_OBJ)
	$(DIRECTORY_GUARD)
	$(AR) $(ARFLAGS) $@ $^

$(LIBCOMPAT): $(LIBCOMPAT_OBJ)
	$(DIRECTORY_GUARD)
	$(AR) $(ARFLAGS) $@ $^

$(LIBSYSTEM): $(LIBSYSTEM_OBJ)
	$(DIRECTORY_GUARD)
	$(AR) $(ARFLAGS) $@ $^

# --- Application ------------------------------------------------------------ #

$(ROOT_DIRECTORY)/bin/Terminal: $(wildcard application/Terminal/*.c) $(LIBTERMINAL) $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $(wildcard application/Terminal/*.c) -o $@ -lterminal -lgraphic


$(ROOT_DIRECTORY)/bin/Shell: $(wildcard application/Shell/*.c) $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $(wildcard application/Shell/*.c) -o $@

$(ROOT_DIRECTORY)/bin/Compositor: $(wildcard application/Compositor/*.c) $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $(wildcard application/Compositor/*.c) -o $@ -lgraphic

# --- Userspace -------------------------------------------------------------- #

$(ROOT_DIRECTORY)/bin/__democolors: userspace/__democolors.c $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

$(ROOT_DIRECTORY)/bin/__demofonts: userspace/__demofonts.c $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

$(ROOT_DIRECTORY)/bin/__demolines: userspace/__demolines.c $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

$(ROOT_DIRECTORY)/bin/__test3d: userspace/__test3d.c $(LIBSYSTEM) $(LIBGRAPHIC) $(LIBMATH) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@ -lgraphic -lmath

$(ROOT_DIRECTORY)/bin/__testapp: userspace/__testapp.c $(LIBSYSTEM) $(LIBWIDGET) $(LIBGRAPHIC) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@ -lwidget -lgraphic

$(ROOT_DIRECTORY)/bin/__testargs: userspace/__testargs.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/__testexec: userspace/__testexec.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/__testgfx: userspace/__testgfx.c $(LIBSYSTEM) $(LIBGRAPHIC) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

$(ROOT_DIRECTORY)/bin/__testposix: userspace/__testposix.c $(LIBSYSTEM) $(LIBCOMPAT) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@ -lcompat

$(ROOT_DIRECTORY)/bin/__testterm: userspace/__testterm.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/cat: userspace/cat.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/clear: userspace/clear.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/dstart: userspace/dstart.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/echo: userspace/echo.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/displayctl: userspace/displayctl.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/grep: userspace/grep.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/init: userspace/init.c $(LIBGRAPHIC) $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@ -lgraphic

$(ROOT_DIRECTORY)/bin/kill: userspace/kill.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/keyboardctl: userspace/keyboardctl.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/ls: userspace/ls.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/lsproc: userspace/lsproc.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/mkdir: userspace/mkdir.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/mv: userspace/mv.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/now: userspace/now.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/panic: userspace/panic.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/sysfetch: userspace/sysfetch.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/touch: userspace/touch.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

$(ROOT_DIRECTORY)/bin/unlink: userspace/unlink.c $(LIBSYSTEM) $(CRTS)
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) $< -o $@

# --- Kernel ----------------------------------------------------------------- #

%.c.kernel.o: %.c
	$(CC) $(CFLAGS) -ffreestanding -nostdlib -c -o $@ $^

%.s.kernel.o: %.s
	$(AS) $(ASFLAGS) $^ -o $@

$(KERNEL): $(KERNEL_OBJECT)
	$(DIRECTORY_GUARD)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(KERNEL_OBJECT)

# --- Ressources ------------------------------------------------------------- #

WALLPAPERS = ${patsubst ressources/%,$(ROOT_DIRECTORY)/res/%,${wildcard ressources/wallpaper/*.png}}
CURSOR = ${patsubst ressources/%,$(ROOT_DIRECTORY)/res/%,${wildcard ressources/mouse/*.png}}

FONTS_GLYPHS = ${patsubst ressources/%.json,$(ROOT_DIRECTORY)/res/%.glyph,${wildcard ressources/font/*.json}}
FONTS_PNGS = ${patsubst ressources/%,$(ROOT_DIRECTORY)/res/%,${wildcard ressources/font/*.png}}

KEYBOARD = ${patsubst ressources/%.json,$(ROOT_DIRECTORY)/res/%.kmap,${wildcard ressources/keyboard/*.json}}

RESSOURCES = $(WALLPAPERS) $(CURSOR) $(KEYBOARD) $(FONTS_GLYPHS) $(FONTS_PNGS)

$(ROOT_DIRECTORY)/res/font/%.glyph: ressources/font/%.json
	$(DIRECTORY_GUARD)
	font-compiler.py $^ $@

$(ROOT_DIRECTORY)/res/font/%.png: ressources/font/%.png
	$(DIRECTORY_GUARD)
	cp $^ $@

$(ROOT_DIRECTORY)/res/wallpaper/%.png: ressources/wallpaper/%.png
	$(DIRECTORY_GUARD)
	cp $^ $@

$(ROOT_DIRECTORY)/res/mouse/%.png: ressources/mouse/%.png
	$(DIRECTORY_GUARD)
	cp $^ $@

$(ROOT_DIRECTORY)/res/keyboard/%.kmap: ressources/keyboard/%.json
	$(DIRECTORY_GUARD)
	kmap-compiler.py $^ $@

# --- Ramdisk ---------------------------------------------------------------- #

$(ROOT_DIRECTORY)/lib/include/%.h: libraries/%.h
	$(DIRECTORY_GUARD)
	cp $^ $@

$(ROOT_DIRECTORY)/lib/include/%.h: libraries/libcompat/%.h
	$(DIRECTORY_GUARD)
	cp $^ $@

$(ROOT_DIRECTORY):
	mkdir -p $(ROOT_DIRECTORY)/bin \
			 $(ROOT_DIRECTORY)/dev \
			 $(ROOT_DIRECTORY)/etc \
			 $(ROOT_DIRECTORY)/lib \
			 $(ROOT_DIRECTORY)/res \
			 $(ROOT_DIRECTORY)/run \
			 $(ROOT_DIRECTORY)/srv \
			 $(ROOT_DIRECTORY)/usr

$(RAMDISK): $(ROOT_DIRECTORY) $(INCLUDES) $(LIBRARIES) $(USERSPACE) $(APPLICATION) $(RESSOURCES)
	cd $(ROOT_DIRECTORY); tar -cf $@ *

# --- Bootdisk --------------------------------------------------------------- #

$(BOOT_DIRECTORY): grub.cfg $(KERNEL) $(RAMDISK)
	mkdir -p $(BOOT_DIRECTORY)/boot/grub
	cp grub.cfg $(BOOT_DIRECTORY)/boot/grub/
	cp $(KERNEL) $(BOOT_DIRECTORY)/boot
	cp $(RAMDISK) $(BOOT_DIRECTORY)/boot
	touch $(BOOT_DIRECTORY)

$(BOOTDISK): $(BOOT_DIRECTORY)
	grub-mkrescue -o $@ $(BOOT_DIRECTORY) || \
	grub2-mkrescue -o $@ $(BOOT_DIRECTORY)

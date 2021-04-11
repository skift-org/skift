.SUFFIXES:

ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)

OBJS            = main.o
LOADER_BINARY          = loader.efi

EFI_INCLUDE  = /usr/include/efi
EFI_LIBRARY  = /usr/lib
EFI_CRT_OBJS = $(EFI_LIBRARY)/crt0-efi-x86_64.o
EFI_LDSCRIPT = $(EFI_LIBRARY)/elf_x86_64_efi.lds

CXXFLAGS = \
	-nostdlib \
	-fno-stack-protector \
	-fno-strict-aliasing \
	-fno-builtin \
	-fpic \
	-fshort-wchar \
	-mno-red-zone \
	-Wall \
	-Wextra \
	-Werror

CXXFLAGS        += -DEFI_FUNCTION_WRAPPER

CXXFLAGS          += \
				   -I../ \
				   -I$(EFI_INCLUDE) \
				   -I$(EFI_INCLUDE)/x86_64 \
				   -I$(EFI_INCLUDE)/protocol

LDFLAGS         = -nostdlib \
				  -znocombreloc \
				  -shared \
				  -no-undefined \
				  -Bsymbolic

LDFLAGS         += -T $(EFI_LDSCRIPT) \
				   -L$(EFI_LIBRARY) \
				   $(EFI_CRT_OBJS)

LIBS            = -lefi \
				  -lgnuefi

OBJCOPYFLAGS    = -j .text \
				  -j .sdata \
				  -j .data \
				  -j .dynamic \
				  -j .dynsym \
				  -j .rel \
				  -j .rela \
				  -j .reloc \
				  --target=efi-app-x86_64

all: $(LOADER_BINARY)

%.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ $(LIBS)

%.efi: %.so
	objcopy $(OBJCOPYFLAGS) $^ $@

.PHONY:    clean

clean:
	rm -f $(OBJS) $(LOADER_BINARY) loader.so

LIBRARIES = $(shell echo */)

install: $(LIBRARIES)
	$(AS) $(ASFLAGS) crt0.s -o $(SYSROOT)/lib/crt0.o
	$(AS) $(ASFLAGS) crti.s -o $(SYSROOT)/lib/crti.o
	$(AS) $(ASFLAGS) crtn.s -o $(SYSROOT)/lib/crtn.o

list:
	@echo $(LIBRARIES)

.PHONY: $(LIBRARIES)

clean:
	make -C libc clean
	make -C libgfx clean
	make -C libvtc clean

$(LIBRARIES) : % : %/Makefile
	make -C $@ install
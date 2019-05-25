AS=nasm
CSOURCES=$(wildcard *.c) $(wildcard */*.c)
SSOURCES=$(wildcard *.s) $(wildcard */*.s)
OBJECTS= ${CSOURCES:.c=.o} ${SSOURCES:.s=.o}

all: kernel.bin

clean:
	rm -f $(OBJECTS) $(OBJECTS)
	rm -f kernel.bin

install: all
	cp kernel.bin $(BOOTROOT)/boot/kernel.bin

kernel.bin: $(OBJECTS)
	$(LD) -T kernel.ld -o kernel.bin $(OBJECTS) $(SYSROOT)/lib/libvtc.a $(SYSROOT)/lib/libk.a

%.o: %.c
	$(CC) -ffreestanding -nostdlib -c -o $@ $^

%.o: %.s
	$(AS) -f elf32 $^ -o $@
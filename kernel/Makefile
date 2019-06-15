KERNEL=kernel.bin

CSOURCES=$(wildcard *.c) $(wildcard */*.c)
SSOURCES=$(wildcard *.s) $(wildcard */*.s)
OBJECTS= ${CSOURCES:.c=.o} ${SSOURCES:.s=.o}

CFLAGS += -ffreestanding -nostdlib

all: $(KERNEL)

clean:
	rm -f $(OBJECTS) $(OBJECTS)
	rm -f $(KERNEL)

install: all
	cp $(KERNEL) $(BOOTROOT)/boot/$(KERNEL)

$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(OBJECTS) O$(SYSROOT)/lib/libk.a

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.s
	$(AS) $(ASFLAGS) $^ -o $@
SOURCES = $(wildcard *.c)
DRIVERS = ${SOURCES:.c=}

all: $(DRIVERS)

clean:
	rm -f $(DRIVERS)

list:
	@echo $(DRIVERS)

install: $(DRIVERS)
	cp $(DRIVERS) $(SYSROOT)/bin/

$(DRIVERS) : % : %.c

conhost: conhost.c
	$(CC) $(CFLAGS) $^ -lvtc -o $@

.PHONY: all clean list install
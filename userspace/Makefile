SOURCES = $(wildcard *.c)
BINARIES = ${SOURCES:.c=.out}
INSTALL = ${SOURCES:.c=.install}

all: $(BINARIES)

clean:
	rm -f $(BINARIES)

list:
	@echo $(BINARIES)

install: $(INSTALL)

%.out : %.c
	$(CC) $(CFLAGS) $^ -o $@

-include *.mk

%.install: %.out
	cp $^ $(SYSROOT)/bin/$(patsubst %.out,%,$^)


.PHONY: all clean list install
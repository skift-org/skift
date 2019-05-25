SOURCES = $(wildcard *.c)
COREUTILS = ${SOURCES:.c=}

all: $(COREUTILS)

clean:
	rm -f $(COREUTILS)

list:
	@echo $(COREUTILS)

install: $(COREUTILS)
	cp $(COREUTILS) $(SYSROOT)/bin/

$(COREUTILS) : % : %.c

.PHONY: all clean list install
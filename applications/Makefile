SOURCES = $(wildcard *.c)
APPS = ${SOURCES:.c=}

all: $(APPS)

clean:
	rm -f $(APPS)

list:
	@echo $(APPS)

install: $(APPS)
	cp $(APPS) $(SYSROOT)/bin/

$(APPS) : % : %.c

terminal: terminal.c
	$(CC) $(CFLAGS) $^ -lvtc -o $@

.PHONY: all clean list install
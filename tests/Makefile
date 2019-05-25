SOURCES = $(wildcard *.c)
TESTS = ${SOURCES:.c=}

all: $(TESTS)

clean:
	rm -f $(TESTS)

list:
	@echo $(TESTS)

install: $(TESTS)
	cp $(TESTS) $(SYSROOT)/bin/

$(TESTS) : % : %.c

.PHONY: all clean list install
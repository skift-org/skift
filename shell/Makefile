SOURCES = $(wildcard *.c)

sh.out: $(SOURCES)
	$(CC) $^ -o $@

clean:
	rm -f sh.out

install: sh.out
	cp sh.out $(SYSROOT)/bin/sh
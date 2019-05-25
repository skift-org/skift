SOURCES = $(wildcard *.c)

sh: $(SOURCES)
	$(CC) $^ -o $@

clean:
	rm -f sh

install: sh
	cp sh $(SYSROOT)/bin/
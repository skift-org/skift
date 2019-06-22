SOURCES = $(wildcard *.c)

hideo.out: $(SOURCES)
	$(CC) $(CFLAGS) $^ -lgfx -o $@

clean:
	rm -f hideo.out

install: hideo.out
	cp hideo.out $(SYSROOT)/bin/hideo
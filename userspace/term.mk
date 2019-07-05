term.out: term.c
	$(CC) $(CFLAGS) $^ -lvtc -lgfx -o $@
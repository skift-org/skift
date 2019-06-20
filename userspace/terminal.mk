terminal.out: terminal.c
	$(CC) $(CFLAGS) $^ -lvtc -lgfx -o $@
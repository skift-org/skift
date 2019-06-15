lines.out: lines.c
	$(CC) $(CFLAGS) $^ -lgfx -o $@
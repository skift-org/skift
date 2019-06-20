fonts.out: fonts.c
	$(CC) $(CFLAGS) $^ -lgfx -o $@
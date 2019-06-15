colors.out: colors.c
	$(CC) $(CFLAGS) $^ -lgfx -o $@
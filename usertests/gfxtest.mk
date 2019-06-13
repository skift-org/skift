gfxtest.out: gfxtest.c
	$(CC) $(CFLAGS) $^ -lgfx -o $@
wm.out: wm.c
	$(CC) $(CFLAGS) $^ -lgfx -o $@
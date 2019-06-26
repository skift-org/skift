testapp.out: testapp.c
	$(CC) $(CFLAGS) $^ -lgui -lgfx -o $@
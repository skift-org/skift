terminal.out: terminal.c
	$(CC) $(CFLAGS) $^ -lvtc -o $@
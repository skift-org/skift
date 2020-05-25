#pragma once

#include <libsystem/Common.h>

char serial_getc(void);
void serial_putc(char c);

int serial_write(const void *buffer, uint size);

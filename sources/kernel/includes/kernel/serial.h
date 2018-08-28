#pragma once

void serial_setup();

void serial_putc(char c);
char serial_getc();
void serial_print(const char *str);
void serial_read(char *buffer, int size);
#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

void serial_setup();

void serial_putc(char c);
char serial_getc();
void serial_writeln(const char *str);
void serial_read(char *buffer, int size);
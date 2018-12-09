#pragma once
#include <skift/generic.h>
// Framework plugs to the syscalls or the kernel.

void __plug_print(const char *buffer);
void __plug_putchar(char c);
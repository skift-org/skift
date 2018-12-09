#pragma once

#include <skift/types.h>
#include <skift/utils.h>
#include <skift/ringbuffer.h>

void sk_console_print(const char *msg);
void sk_console_println(const char *msg);
void sk_console_printf(const char *fmt, ...);

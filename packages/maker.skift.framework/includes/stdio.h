#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdarg.h>

#include "defs/NULL.h"
#include "defs/size_t.h"

/* --- Stdin/Stout ---------------------------------------------------------- */

int putchar(int chr);
int getchar();

char *gets(char *str);
int puts(const char *str);

/* --- Printf --------------------------------------------------------------- */
int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list va);
int sprintf(char *s, const char *fmt, ...);
int vsprintf(char *s, const char *fmt, va_list va);
int snprintf(char *s, int n, const char *fmt, ...);
int vsnprintf(char *s, size_t n, const char *fmt, va_list va);
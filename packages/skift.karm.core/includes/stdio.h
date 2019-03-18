#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdarg.h>

#include "defs/NULL.h"
#include "defs/size_t.h"

#define EOF -1
#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

/* --- Files operations ----------------------------------------------------- */

typedef struct
{
    int handle;
} FILE;

FILE *stdin;
FILE *stdout;
FILE *stderr;

FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

int fseek(FILE *stream, long int offset, int whence);
long int ftell(FILE *stream);

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
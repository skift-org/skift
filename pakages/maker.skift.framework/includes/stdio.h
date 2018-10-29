#pragma once
#include "defs/NULL.h"
#include "defs/size_t.h"

#include <stdarg.h>

#define EOF -1

typedef int FILE;

int fclose(FILE *stream);
FILE *fopen(const char *filename, const char *mode);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

long int ftell(FILE *stream);
int fseek(FILE *stream, long int offset, int whence);

// Standart-ish
int sprintf(char *str, const char *format, ...);
int printf(const char *format, ...);
int vprintf(const char *format, va_list va);
int vsprintf(char *str, const char *format, va_list va);

int putchar(int chr);
int getchar();

char *gets(char *str);
int puts(const char *str);

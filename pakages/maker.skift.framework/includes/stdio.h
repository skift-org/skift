#pragma once
#include "defs/NULL.h"
#include "defs/size_t.h"

#include <stdarg.h>

#define EOF -1
#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

/* --- Files operations ----------------------------------------------------- */

typedef struct 
{
    int handle;
} FILE;

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
int printf(const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char* s, const char * fmt, ...);

int vprintf(const char *format, va_list va);
int vsprintf(char *str, const char *format, va_list va);
int vsnprintf(char* s, size_t n, const char * fmt, va_list va);
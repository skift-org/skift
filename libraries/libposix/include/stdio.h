#pragma once

#include <skift/runtime.h>

#define EOF -1
#define BUFSIZ 512

#define _IONBF 1
#define _IOFBF 2
#define _IOLBF 3

#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

typedef void FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

FILE *tmpfile(void);

FILE *freopen(const char *path, const char *mode, FILE *stream);
FILE *fopen(const char *path, const char *mode);
int fclose(FILE *stream);

int setvbuf(FILE *stream, char *buf, int mode, size_t size);

void clearerr(FILE *stream);
int ferror(FILE *stream);
int feof(FILE *stream);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fflush(FILE *stream);

char *fgets(char *s, int size, FILE *stream);
int getc(FILE *stream);
int ungetc(int c, FILE *stream);

int fprintf(FILE *stream, const char *format, ...);

int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
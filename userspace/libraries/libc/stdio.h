#pragma once

#include <__libc__.h>

#include <stdarg.h>
#include <stddef.h>

__BEGIN_HEADER

typedef struct
{
    int handle;
    int is_eof;
    int error;
} FILE;
#define __DEFINED_FILE

#define BUFSIZ 8192

FILE *__stdio_get_stdin(void);
FILE *__stdio_get_stdout(void);
FILE *__stdio_get_stderr(void);
FILE *__stdio_get_stdlog(void);

#define stdin (__stdio_get_stdin())
#define stdout (__stdio_get_stdout())
#define stderr (__stdio_get_stderr())
#define stdlog (__stdio_get_stdlog())

#define EOF (-1)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *stream);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

int fileno(FILE *stream);
int fflush(FILE *stream);

int vasprintf(char **buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
int fprintf(FILE *stream, const char *fmt, ...);
int printf(const char *fmt, ...);
int snprintf(char *buf, size_t size, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vprintf(const char *format, va_list ap);

int puts(const char *s);
int fputs(const char *s, FILE *stream);
int fputc(int c, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
int fgetc(FILE *stream);
int getc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int getchar(void);

void rewind(FILE *stream);
void setbuf(FILE *stream, char *buf);

void perror(const char *s);

int ungetc(int c, FILE *stream);

int feof(FILE *stream);
void clearerr(FILE *stream);
int ferror(FILE *stream);

char *strerror(int errnum);

int _fwouldblock(FILE *stream);

FILE *tmpfile(void);

int setvbuf(FILE *stream, char *buf, int mode, size_t size);

int remove(const char *pathname);
int rename(const char *oldpath, const char *newpath);

#define _IONBF 0
#define _IOLBF 1
#define _IOFBF 2

char *tmpnam(char *s);
char *tempnam(const char *dir, const char *s);
#define L_tmpnam 256

int vsscanf(const char *str, const char *format, va_list ap);
int sscanf(const char *str, const char *format, ...);
int vfscanf(FILE *stream, const char *format, va_list ap);
int fscanf(FILE *stream, const char *format, ...);
int scanf(const char *format, ...);

typedef long fpos_t;

int fgetpos(FILE *stream, fpos_t *pos);
int fsetpos(FILE *stream, const fpos_t *pos);

__END_HEADER

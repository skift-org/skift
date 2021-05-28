#pragma once

#include <__libc__.h>

#include <stddef.h>

#include <bits/abs.h>

__BEGIN_HEADER

void exit(int status);
char *getenv(const char *name);

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void malloc_cleanup(void *buffer);

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

int system(const char *command);

int putenv(char *name);
int setenv(const char *name, const char *value, int overwrite);
int unsetenv(const char *str);

double strtod(const char *nptr, char **endptr);
float strtof(const char *nptr, char **endptr);
double atof(const char *nptr);
int atoi(const char *nptr);
long atol(const char *nptr);
long int strtol(const char *s, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
unsigned long long int strtoull(const char *nptr, char **endptr, int base);

void srand(unsigned int);
int rand(void);

#define ATEXIT_MAX 32
int atexit(void (*h)(void));
void _handle_atexit(void);

#define RAND_MAX 0x7FFFFFFF

void abort();

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void *bsearch(
    const void *key,
    const void *base,
    size_t nmemb,
    size_t size,
    int (*compar)(const void *, const void *));

char *mktemp(char *);
int mkstemp(char *);

size_t mbstowcs(wchar_t *dest, const char *src, size_t n);
size_t wcstombs(char *dest, const wchar_t *src, size_t n);

typedef struct
{
    int quot;
    int rem;
} div_t;
typedef struct
{
    long int quot;
    long int rem;
} ldiv_t;

div_t div(int numerator, int denominator);
ldiv_t ldiv(long numerator, long denominator);

/* These are supposed to be in limits, but gcc screwed us */
#define PATH_MAX 4096
#define NAME_MAX 255
char *realpath(const char *path, char *resolved_path);

__END_HEADER

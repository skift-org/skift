#pragma once

#include <__libc__.h>

#include <stddef.h>
#include <stdint.h>

__BEGIN_HEADER

void *memset(void *dest, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);

void *memchr(const void *src, int c, size_t n);
void *memrchr(const void *m, int c, size_t n);
int memcmp(const void *vl, const void *vr, size_t n);

char *strdup(const char *s);
char *stpcpy(char *d, const char *s);
char *strcpy(char *dest, const char *src);
char *strchrnul(const char *s, int c);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strpbrk(const char *s, const char *b);
char *strstr(const char *h, const char *n);

char *strncpy(char *dest, const char *src, size_t n);

int strcmp(const char *l, const char *r);
int strncmp(const char *s1, const char *s2, size_t n);
int strcoll(const char *s1, const char *s2);

size_t strcspn(const char *s, const char *c);
size_t strspn(const char *s, const char *c);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);

int atoi(const char *s);

char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);

char *strtok(char *str, const char *delim);
char *strtok_r(char *str, const char *delim, char **saveptr);

char *strncpy(char *dest, const char *src, size_t n);

char *strerror(int errnum);
size_t strxfrm(char *dest, const char *src, size_t n);

// Skift extensions, will be removed
void strrvs(char *str);
void strnapd(char *str, char c, size_t n);
void strapd(char *str, char c);
size_t strlcpy(char *dst, const char *src, size_t maxlen);

__END_HEADER

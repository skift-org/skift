#pragma once

#include <__libc__.h>

#include <libsystem/Common.h>

__BEGIN_HEADER

void strnapd(char *str, char c, size_t n);
void strapd(char *str, char c);
void strbs(char *str);
void strrvs(char *str);
void strleadtrim(char *str, char c);
void strtrailtrim(char *str, char c);

int snprintf(char *s, size_t n, const char *fmt, ...);
int vsnprintf(char *s, size_t n, const char *fmt, va_list va);

// STANDART
void *memchr(const void *str, int c, size_t n);
int memcmp(const void *str1, const void *str2, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *str, int c, size_t n);

char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
size_t strlcat(char *dst, const char *src, size_t maxlen);

int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);

size_t strlen(const char *str);
size_t strnlen(const char *s, size_t maxlen);

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
size_t strlcpy(char *dst, const char *src, size_t maxlen);

char *strdup(const char *s);
char *strchr(const char *str, int c);
int strcoll(const char *str1, const char *str2);
size_t strcspn(const char *str1, const char *str2);
char *strpbrk(const char *str1, const char *str2);
char *strrchr(const char *str, int c);
size_t strspn(const char *str1, const char *str2);
char *strstr(const char *haystack, const char *needle);
char *strtok(char *s, const char *delim);
char *strtok_r(char *s, const char *delim, char **state);
size_t strxfrm(char *dest, const char *src, size_t n);

__END_HEADER

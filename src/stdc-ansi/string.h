#pragma once

#include <stdc-base/prelude.h>

/* --- 7.24 - String handling ----------------------------------------------- */

#include <stddef.h>

STDC_BEGIN_HEADER

/* --- 7.24.2 - Copying functions ------------------------------------------- */

void *memcpy(void *STDC_RESTRICT s1, void const *STDC_RESTRICT s2, size_t n);

void *memmove(void *s1, void const *s2, size_t n);

char *strcpy(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2);

char *strncpy(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2, size_t n);

/* --- 7.24.3 - Concatenation functions ------------------------------------- */

char *strcat(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2);

char *strncat(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2, size_t n);

/* --- 7.24.4 - Comparison functions ---------------------------------------- */

int memcmp(void const *s1, void const *s2, size_t n);

int strcmp(char const *s1, char const *s2);

int strcoll(char const *s1, char const *s2);

int strncmp(char const *s1, char const *s2, size_t n);

size_t strxfrm(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2, size_t n);

/* --- 7.24.5 - Search functions -------------------------------------------- */

void *memchr(void const *s, int c, size_t n);

char *strrchr(char const *s, int c);

size_t strcspn(char const *s1, char const *s2);

char *strpbrk(char const *s1, char const *s2);

char *strrchr(char const *s, int c);

size_t strspn(char const *s1, char const *s2);

char *strstr(char const *s1, char const *s2);

char *strtok(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2);

/* --- 7.24.6 - Miscellaneous functions ------------------------------------- */

void *memset(void *s, int c, size_t n);

char *strerror(int errnum);

size_t strlen(char const *s);

STDC_END_HEADER

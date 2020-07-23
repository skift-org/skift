#pragma once

#include <__libc__.h>

#include <stddef.h>

__BEGIN_HEADER

__attribute__((malloc)) __attribute__((alloc_size(1))) void *malloc(size_t size);
__attribute__((malloc)) __attribute__((alloc_size(1, 2))) void *calloc(size_t, size_t);

void *realloc(void *p, size_t size);

void free(void *);

void malloc_cleanup(void *buffer);

__END_HEADER

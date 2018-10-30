#pragma once

#include <skift/types.h>
#include <skift/utils.h>

#define malloc(size) sk_memory_alloc(size)
#define free(addr) sk_memory_free(addr)
#define memcpy(src, dest, size) sk_memory_copy((src), (dest), (size))
#define memmove(src, dest, size) sk_memory_move((src), (dest), (size))
#define memset(addr, value, size) sk_memory_set((addr), (size), (value))
#define memzero(addr, size) sk_memory_zero((addr), (size))

void *memory_alloc(uint size);
void memory_free(void *addr);

void memory_copy(void *src, void *dest, uint size);
void memory_move(void *src, void *dest, uint size);
void memory_set(void *addr, uint size, byte value);
void memory_zero(void *addr, uint size);
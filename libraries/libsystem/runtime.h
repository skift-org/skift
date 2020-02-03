#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NULL ((void *)0)

#define let __auto_type const
#define var __auto_type

typedef void s0;
typedef char s8;
typedef short s16;
typedef long s32;
typedef long long s64;

typedef void u0;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef char byte;
typedef unsigned char ubyte;
typedef unsigned char uchar;
typedef unsigned short wchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef long off_t;

#define __packed __attribute__((packed))

#define __aligned(__align) __attribute__((aligned(__align)))

#define __unused(__stuff) (void)(__stuff)

#define __create(__type) ((__type *)calloc(1, sizeof(__type)))

#define SWAP(x, y)          \
    ({                      \
        typeof(x) SWAP = x; \
        x = y;              \
        y = SWAP;           \
    })

/* --- Raw memory allocation ------------------------------------------------ */

__attribute__((malloc)) __attribute__((alloc_size(1))) void *malloc(size_t size);
__attribute__((malloc)) __attribute__((alloc_size(1, 2))) void *calloc(size_t, size_t);

void *realloc(void *p, size_t size);

void free(void *);

/* --- Refcounted object runtime -------------------------------------------- */

typedef void (*object_dtor_t)(void *object);

typedef void object_t;

#define OBJECT(__type) (__type##_t *)object(sizeof(__type##_t), (object_dtor_t)(__type##_delete));

object_t *object(uint size, object_dtor_t dtor);

void object_lock(object_t *this);
bool object_trylock(object_t *this);
void object_unlock(object_t *this);

object_t *object_retain(object_t *this);
void object_release(object_t *this);
int object_refcount(object_t *this);
int object_size(object_t *this);

/* --- Loops ---------------------------------------------------------------- */

typedef enum
{
    ITERATION_CONTINUE,
    ITERATION_STOP,
} IterationDecision;
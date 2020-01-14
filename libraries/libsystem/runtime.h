#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdarg.h>
#include <stddef.h>

#define NULL ((void *)0)

#define bool _Bool
#define true 1
#define false 0

#define let __auto_type const
#define var __auto_type

typedef void s0;
typedef char s8;
typedef short s16;
typedef long s32;
typedef long long s64;

typedef void u0;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef char byte;
typedef unsigned char ubyte;
typedef unsigned char uchar;
typedef unsigned short wchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef signed char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef long off_t;
typedef long intptr_t;
typedef unsigned long uintptr_t;

#define __packed __attribute__((packed))

#define __aligned(__align) __attribute__((aligned(__align)))

#define __unused(__stuff) (void)(__stuff)

#define __create(__type) ((__type *)calloc(1, sizeof(__type)))

#define __malloc __attribute__((malloc))

#define SWAP(x, y)          \
    {                       \
        typeof(x) SWAP = x; \
        x = y;              \
        y = SWAP;           \
    }

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define FLAG(__i) (1 << (__i))

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
} iteration_decision_t;
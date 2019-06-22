#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stddef.h>
#include <stdarg.h>

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

typedef long intptr_t;
typedef unsigned long uintptr_t;

#define attr_packed __attribute__((packed))
#define ALIGNED(x, align) x __attribute__((aligned(align)))

#define UNUSED(x) (void)(x)
#define MALLOC(type) ((type *)malloc(sizeof(type)))

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define FLAG(__i) (1 << (__i))

#define offsetof(st, m) ((size_t) & (((st *)0)->m))

/* --- Automagicaly variables filler ---------------------------------------- */

#ifndef __PACKAGE__
#define __PACKAGE__ "(NULL)"
#endif

/* --- Raw memory allocation ------------------------------------------------ */

void *malloc(size_t size);
void *realloc(void *p, size_t size);
void *calloc(size_t, size_t);
void free(void *);

/* --- Refcounted object runtime -------------------------------------------- */

typedef void (*object_dtor_t)(void *object);

typedef void object_t;

#define OBJECT(__type) (__type##_t*)object(sizeof(__type##_t), (object_dtor_t)(__type##_delete));

object_t *object(uint size, object_dtor_t dtor);

void object_lock(object_t *this);
bool object_trylock(object_t *this);
void object_unlock(object_t *this);

object_t *object_retain(object_t *this);
void object_release(object_t *this);
int object_refcount(object_t *this);
int object_size(object_t *this);

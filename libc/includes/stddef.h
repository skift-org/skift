#pragma once

#define NULL ((void*)0)
#define offsetof(st, m) ((size_t)&(((st *)0)->m))

typedef unsigned int size_t;
typedef long long ptrdiff_t;

#ifndef __cplusplus
typedef unsigned int wchar_t;
#endif
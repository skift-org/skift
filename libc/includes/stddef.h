#pragma once

#include "defs/NULL.h"
#include "defs/size_t.h"

#define offsetof(st, m) ((size_t)&(((st *)0)->m))

typedef long long ptrdiff_t;

#ifndef __cplusplus
typedef unsigned int wchar_t;
#endif
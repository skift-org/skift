#pragma once

#include <stdio.h>

#define PACKED(x) x __attribute__((packed))
#define ALIGNED(x, align) x __attribute__((aligned(align)))
#define UNUSED(x) (void)(x)
#define MALLOC(type) ((type *)malloc(sizeof(type)))
#define STUB(a...) printf("STUB REACHED %s\n", __FUNCTION__, a)
#define ARRAY_SIZE(array)  (sizeof(array) / sizeof(array[0]))
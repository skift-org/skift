#pragma once

#include <libsystem/runtime.h>

typedef struct
{
    uint64_t state;
    uint64_t inc;
} Random;

Random random_create(void);

uint32_t random_uint32(Random *random);
uint32_t random_uint32_max(Random *random, uint32_t max);

double random_double(Random *random);
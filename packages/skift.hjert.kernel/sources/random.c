/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>

#include "kernel/filesystem.h"

static int state = 1411743402;

int random_read(stream_t *s, uint size, void *buffer)
{
    UNUSED(s);

    byte* b = buffer;
    
    for(uint i = 0; i < size; i++)
    {
        uint x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        state = x;

        b[i] = (byte)x;
    }
    
    
    return size;
}

int random_write(stream_t *s, uint size, void *buffer)
{
    UNUSED(s);
    UNUSED(buffer);
    return size;
}

static device_t random;

void random_setup(void)
{
    random = (device_t){random_read, random_write, NULL};
    filesystem_mkdev("/dev/random", random);
}
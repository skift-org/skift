/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/cstring.h>

#include "kernel/system.h"
#include "kernel/filesystem.h"

static int state = 1411743402;

int random_device_read(stream_t *s, void *buffer, uint size)
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

int random_device_write(stream_t *s, const void *buffer, uint size)
{
    UNUSED(s); UNUSED(buffer); UNUSED(size);

    return 0;
}

void random_setup(void)
{
    device_t random_device = {
        .read = random_device_read,
        .write = random_device_write,
    };

    FILESYSTEM_MKDEV("random", random_device);
}
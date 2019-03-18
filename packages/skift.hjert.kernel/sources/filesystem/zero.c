/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>

#include "kernel/system.h"
#include "kernel/filesystem.h"

int zero_device_read(stream_t *s, void *buffer, uint size)
{
    UNUSED(s);

    memset(buffer, 0, size);
    
    return size;
}

int zero_device_write(stream_t *s, void *buffer, uint size)
{
    UNUSED(s);
    UNUSED(buffer);
    return size;
}


void zero_setup(void)
{
    device_t zero_device = 
    {
        .read = zero_device_read,
        .write = zero_device_write
    };

    FILESYSTEM_MKDEV("zero", zero_device);
}
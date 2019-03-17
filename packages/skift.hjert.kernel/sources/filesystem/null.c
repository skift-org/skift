/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>

#include "kernel/system.h"
#include "kernel/filesystem.h"

int null_device_read(stream_t *s, void *buffer, uint size)
{
    UNUSED(s);
    UNUSED(size);
    UNUSED(buffer);

    return 0;
}

int null_device_write(stream_t *s, void *buffer, uint size)
{
    UNUSED(s);
    UNUSED(buffer);

    return size;
}

void null_setup(void)
{
    device_t null_device = 
    {
        .read = null_device_read,
        .write = null_device_write
    };

    FILESYSTEM_MKDEV("null", null_device);
}
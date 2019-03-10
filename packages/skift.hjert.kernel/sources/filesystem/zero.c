/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>

#include "kernel/system.h"
#include "kernel/filesystem.h"

int zero_read(stream_t *s, void *buffer, uint size)
{
    UNUSED(s);

    memset(buffer, 0, size);
    
    return size;
}

int zero_write(stream_t *s, void *buffer, uint size)
{
    UNUSED(s);
    UNUSED(buffer);
    return size;
}

static device_t zero;

void zero_setup(void)
{
    zero = (device_t){zero_read, zero_write, NULL};
    if (filesystem_mkdev("/Devices/zero", zero))
    {
        PANIC("Failled to create the 'zero' device.");
    }
}
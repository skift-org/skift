/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include "system.h"
#include "filesystem.h"

int zero_device_read(stream_t *s, void *buffer, uint size)
{
    __unused(s);

    memset(buffer, 0, size);

    return size;
}

int zero_device_write(stream_t *s, const void *buffer, uint size)
{
    __unused(s);
    __unused(buffer);
    return size;
}

void zero_setup(void)
{
    device_t zero_device = {
        .read = zero_device_read,
        .write = zero_device_write,
    };

    FILESYSTEM_MKDEV("/dev/zero", zero_device);
}
/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include "system.h"
#include "filesystem.h"

int null_device_read(stream_t *s, void *buffer, uint size)
{
    __unused(s);
    __unused(size);
    __unused(buffer);

    return 0;
}

int null_device_write(stream_t *s, const void *buffer, uint size)
{
    __unused(s);
    __unused(buffer);

    return size;
}

void null_setup(void)
{
    device_t null_device = {
        .read = null_device_read,
        .write = null_device_write,
    };

    FILESYSTEM_MKDEV("/dev/null", null_device);
}
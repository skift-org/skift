#include <string.h>

#include "kernel/filesystem.h"

int zero_read(stream_t *s, uint size, void *buffer)
{
    UNUSED(s);

    memset(buffer, 0, size);
    
    return size;
}

int zero_write(stream_t *s, uint size, void *buffer)
{
    UNUSED(s);
    UNUSED(buffer);
    return size;
}

static device_t zero;

void zero_setup(void)
{
    zero = (device_t){zero_read, zero_write, NULL};
    filesystem_mkdev("/dev/zero", zero);
}
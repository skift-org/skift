#include <unistd.h>

#include <libsystem/core/Plugs.h>

ssize_t write(int fd, const void *buf, size_t count)
{
    Handle hnd = {
        .id = fd,
        .flags = 0,
        .result = SUCCESS,
    };

    size_t written = __plug_handle_write(&hnd, buf, count);

    if (handle_has_error(&hnd))
    {
        return -1;
    }
    else
    {
        return (ssize_t)written;
    }
}

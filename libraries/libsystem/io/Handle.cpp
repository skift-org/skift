#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>

int __handle_printf_error(Handle *handle, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = stream_vprintf(err_stream, fmt, va);

    va_end(va);

    stream_format(err_stream, ": %s\n", handle_error_string(handle));

    return result;
}

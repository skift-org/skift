
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

Result handle_poll(
    Handle **handles,
    PollEvent *events,
    size_t count,
    Handle **selected,
    PollEvent *selected_events,
    Timeout timeout)
{
    int *handles_index = (int *)calloc(count, sizeof(int));

    for (size_t i = 0; i < count; i++)
    {
        handles_index[i] = handles[i]->id;
    }

    int selected_index = HANDLE_INVALID_ID;

    HandleSet handleset = (HandleSet){handles_index, events, count};

    Result result = __plug_handle_poll(
        &handleset,
        &selected_index,
        selected_events,
        timeout);

    free(handles_index);

    if (result == SUCCESS)
    {
        for (size_t i = 0; i < count; i++)
        {
            if (handles[i]->id == selected_index)
            {
                *selected = handles[i];
            }
        }
    }

    return result;
}

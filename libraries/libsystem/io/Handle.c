#include <libsystem/__plugs__.h>
#include <libsystem/io/Stream.h>

int __handle_printf_error(Handle *handle, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = stream_vprintf(err_stream, fmt, va);

    va_end(va);

    stream_printf(err_stream, ": %s\n", handle_error_string(handle));

    return result;
}

error_t handle_select(Handle **handles, SelectEvent *events, size_t count, Handle **selected, SelectEvent *selected_events)
{
    int *handles_index = calloc(count, sizeof(int));

    for (size_t i = 0; i < count; i++)
    {
        handles_index[i] = handles[i]->id;
    }

    int selected_index = HANDLE_INVALID_ID;
    error_t result = __plug_handle_select(handles_index, events, count, &selected_index, selected_events);

    free(handles_index);

    if (result == ERR_SUCCESS)
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
#pragma once

#include <abi/Handle.h>
#include <skift/Time.h>

#define handle_printf_error(__handle, __args...) __handle_printf_error(HANDLE(__handle), __args)

int __handle_printf_error(Handle *handle, const char *fmt, ...);

Result handle_poll(
    Handle **handles,
    PollEvent *events,
    size_t count,
    Handle **selected,
    PollEvent *selected_events,
    Timeout timeout);

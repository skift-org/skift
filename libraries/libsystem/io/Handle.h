#pragma once

#include <abi/Handle.h>

#include <libsystem/Time.h>

#define handle_printf_error(__handle, __args...) __handle_printf_error(HANDLE(__handle), __args)

int __handle_printf_error(Handle *handle, const char *fmt, ...);

Result handle_select(
    Handle **handles,
    SelectEvent *events,
    size_t count,
    Handle **selected,
    SelectEvent *selected_events,
    Timeout timeout);

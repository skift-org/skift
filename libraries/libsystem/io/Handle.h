#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Handle.h>

#include <libsystem/time.h>

#define handle_printf_error(__handle, __args...) __handle_printf_error(HANDLE(__handle), __args)

int __handle_printf_error(Handle *handle, const char *fmt, ...);

Result handle_select(
    Handle **handles,
    SelectEvent *events,
    size_t count,
    Handle **selected,
    SelectEvent *selected_events,
    Timeout timeout);

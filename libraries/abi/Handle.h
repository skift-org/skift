#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Filesystem.h>

#include <libsystem/Result.h>

typedef enum
{
    SELECT_READ = 1 << 0,
    SELECT_WRITE = 1 << 1,
    SELECT_SEND = 1 << 2,
    SELECT_RECEIVE = 1 << 3,
    SELECT_CONNECT = 1 << 4,
    SELECT_ACCEPT = 1 << 5,
} SelectEvent;

typedef struct
{
    int id;
    OpenFlag flags;
    Result result;
} Handle;

typedef struct
{
    int *handles;
    SelectEvent *events;
    size_t count;
} HandleSet;

#define HANDLE_INVALID_ID (-1)

#define HANDLE(__subclass) ((Handle *)(__subclass))

#define handle_has_error(__handle) (HANDLE(__handle)->result != SUCCESS)

#define handle_error_string(__handle) result_to_string(HANDLE(__handle)->result)

#define handle_get_error(__handle) (HANDLE(__handle)->result)

#define handle_clear_error(__handle) (HANDLE(__handle)->result = SUCCESS)

#define handle_has_flags(__handle, __flags) ((HANDLE(__handle)->flags & (__flags)) == (__flags))
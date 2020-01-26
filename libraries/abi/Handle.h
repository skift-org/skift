#pragma once

#include <libsystem/error.h>

#include "abi/Filesystem.h"

typedef struct
{
    int id;
    OpenFlag flags;
    error_t error;
} Handle;

#define HANDLE_INVALID_ID (-1)

#define HANDLE(__subclass) ((Handle *)(__subclass))

#define handle_has_error(__handle) (HANDLE(__handle)->error != ERR_SUCCESS)

#define handle_error_string(__handle) error_to_string(HANDLE(__handle)->error)

#define handle_get_error(__handle) (HANDLE(__handle)->error)

#define handle_clear_error(__handle) (HANDLE(__handle)->error = ERR_SUCCESS)

#define handle_has_flags(__handle, __flags) ((HANDLE(__handle)->flags & (__flags)) == (__flags))
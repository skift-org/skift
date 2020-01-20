#pragma once

#include <libsystem/error.h>

typedef struct
{
    int id;
    error_t error;
} Handle;

#define HANDLE(__subclass) ((Handle *)(__subclass))

#define handle_has_error(__handle) (HANDLE(__handle)->error != ERR_SUCCESS)

#define handle_get_error(__handle) (HANDLE(__handle)->error)

#define handle_clear_error(__handle) (HANDLE(__handle)->error = ERR_SUCCESS)
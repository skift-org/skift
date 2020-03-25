#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

typedef enum
{
    MESSAGE_TYPE_EVENT,
    MESSAGE_TYPE_REQUEST,
} MessageType;

typedef struct
{
    int from;
    MessageType type;
    size_t size;
} Message;

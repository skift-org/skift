#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#define MSG_SIZE 512

#define MSGLABEL_SIZE 64
#define MSGPAYLOAD_SIZE (MSG_SIZE - sizeof(message_header_t))
#define MSGLABEL(__proto, __class, __type) (__proto ":" __class "." __type)

typedef enum
{
    MESSAGE_TYPE_EVENT,
    MESSAGE_TYPE_REQUEST,
} message_type_t;

typedef struct
{
    int id;
    int from, to;
    message_type_t type;

    int size;
    char label[MSGLABEL_SIZE];
} message_header_t;

typedef struct
{
    message_header_t header;
    char payload[MSGPAYLOAD_SIZE];
} message_t;

#define message(__label, __to)  \
    (message_t)                 \
    {                           \
        .header = {             \
            .to = (__to),       \
            .label = (__label), \
        }                       \
    }

#define message_set_payload(__message, __payload)                      \
    {                                                                  \
        (__message).header.size = sizeof(__payload);                   \
        memcpy(&(__message).payload, &(__payload), sizeof(__payload)); \
    }

#define message_set_payload_ptr(__message, __payload, __payload_size) \
    {                                                                 \
        (__message).header.size = (__payload_size);                   \
        memcpy(&(__message).payload, (__payload), (__payload_size));  \
    }

#define message_payload(__message) ((__message).payload)

#define message_label(__message) ((__message).header.label)

#define message_is(__message, __label) (strcmp(message_label(__message), (__label)) == 0)

#define message_payload_as(__message, __type) (__type *)(&((__message).payload))

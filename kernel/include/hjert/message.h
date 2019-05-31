#pragma once

#include <skift/cstring.h>

#define MSG_SIZE 512

#define MSGLABEL_SIZE 64
#define MSGPAYLOAD_SIZE (MSG_SIZE - sizeof(message_header_t))

typedef struct
{
    int id;
    int from, to;
    int size;
    uint flags;
    char label[MSGLABEL_SIZE];
} message_header_t;

typedef struct
{
    message_header_t header;
    char payload[MSGPAYLOAD_SIZE];
} message_t;

#define message_payload(__message) ((__message).payload)

#define message_label(__message) ((__message).header.label)

#define message_is(__message, __label) (strcmp(message_label(__message), (__label)) == 0)

#define message_payload_as(__message, __type) (__type *)(&(__message).payload)

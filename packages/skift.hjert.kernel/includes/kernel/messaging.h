#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/list.h>

#include "kernel/tasking.h"
#include "kernel/protocol.h"

#define CHANNAME_SIZE 128

typedef struct
{
    char name[CHANNAME_SIZE];
    list_t *subscribers;
} channel_t;

void messaging_setup(void);

int messaging_send(int to, const char *name, void *payload, uint size, uint flags);
int messaging_broadcast(const char *channel, const char *name, void *payload, uint size, uint flags);

message_t* messaging_receive_internal(thread_t* thread);
bool messaging_receive(message_t *msg, bool wait);
int messaging_payload(void *buffer, uint size);

int messaging_subscribe(const char *channel);
int messaging_unsubscribe(const char *channel);

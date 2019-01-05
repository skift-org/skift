#pragma once

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
int messaging_receive(message_t *msg);
int messaging_payload(void *buffer, uint size);

int messaging_subscribe(const char *channel);
int messaging_unsubscribe(const char *channel);

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <math.h>
#include <string.h>
#include <skift/atomic.h>
#include <skift/logger.h>

#include "kernel/tasking.h"
#include "kernel/messaging.h"

static int MID = 1;
static list_t *channels;

/* --- Private functions ---------------------------------------------------- */

channel_t *channel(const char *name)
{
    channel_t *channel = MALLOC(channel_t);

    channel->subscribers = list();
    strncpy(channel->name, name, CHANNAME_SIZE);

    return channel;
}

void channel_delete(channel_t *channel)
{
    list_delete(channel->subscribers);
    free(channel);
}

channel_t *channel_get(const char *channel_name)
{
    FOREACH(i, channels)
    {
        channel_t *c = (channel_t *)i->value;

        if (strcmp(channel_name, c->name) == 0)
            return c;
    }

    return NULL;
}

message_t *message(int id, const char *label, void *payload, uint size, uint flags)
{
    message_t *message = MALLOC(message_t);

    if (payload != NULL && size > 0)
    {
        message->size = min(MSGPAYLOAD_SIZE, size);
        message->payload = malloc(message->size);
        memcpy(message->payload, payload, size);
    }
    else
    {
        message->size = 0;
        message->payload = NULL;
    }

    message->id = id;
    message->flags = flags;
    strncpy(message->label, label, MSGLABEL_SIZE);

    return message;
}

void message_delete(message_t *msg)
{
    if (msg->payload) free(msg->payload);
    free(msg);
}

uint messaging_id()
{
    uint id;

    ATOMIC({
        id = MID++;
    });

    return id;
}

/* --- Public functions ----------------------------------------------------- */
void messaging_setup(void)
{
    channels = list();
}

int messaging_send_internal(PROCESS from, PROCESS to, int id, const char *name, void *payload, uint size, uint flags)
{
    process_t *process = process_get(to);

    if (process == NULL)
    {
        return 0;
    }

    if (process->inbox->count > 1024)
    {
        sk_log(LOG_WARNING, "PROC=%d inbox is full!", to);
        return 0;
    }

    message_t *msg = message(id, name, payload, size, flags);

    msg->from = from;
    msg->to = to;

    list_pushback(process->inbox, (void *)msg);

    FOREACH(t, process->threads)
    {
        thread_t* thread = t->value;

        if (thread->state == THREADSTATE_WAIT_MESSAGE)
        {
            messaging_receive_internal(thread);
            thread_setstate(thread, THREADSTATE_RUNNING);
            break;
        }
    }

    return id;
}

int messaging_send(PROCESS to, const char *name, void *payload, uint size, uint flags)
{
    int id = 0;

    ATOMIC({
        id = messaging_send_internal(process_self(), to, messaging_id(), name, payload, size, flags);
    });

    return id;
}

int messaging_broadcast(const char *channel_name, const char *name, void *payload, uint size, uint flags)
{
    int id = 0;

    sk_atomic_begin();

    channel_t *c = channel_get(channel_name);

    if (c != NULL)
    {
        id = messaging_id();

        FOREACH(p, c->subscribers)
        {
            messaging_send_internal(process_self(), ((process_t *)p->value)->id, id, name, payload, size, flags);
        }
    }

    sk_atomic_end();

    return id;
}

message_t *messaging_receive_internal(thread_t *thread)
{
    message_t *msg = NULL;

    sk_atomic_begin();

    if (thread->process->inbox->count > 0)
    {
        if (thread->wait.message.message != NULL)
        {
            message_delete(thread->wait.message.message);
        }


        list_pop(thread->process->inbox, (void **)&msg);
        thread->wait.message.message = msg;
    }

    sk_atomic_end();
    return msg;
}

bool messaging_receive(message_t *msg, bool wait)
{
    message_t *incoming = messaging_receive_internal(thread_running());

    if (incoming == NULL && wait)
    {
        sk_atomic_begin();
        thread_setstate(thread_running(), THREADSTATE_WAIT_MESSAGE);
        sk_atomic_end(); 

        thread_hold(); // Wait until we get a message.

        incoming = thread_running()->wait.message.message;
    }

    if (incoming != NULL)
    {
        memcpy(msg, incoming, sizeof(message_t));
        return true;
    }

    return false;
}

int messaging_payload(void *buffer, uint size)
{
    message_t *incoming = thread_running()->wait.message.message;

    if (incoming != NULL && incoming->size > 0 && incoming->payload != NULL)
    {
        memcpy(buffer, incoming->payload, min(size, incoming->size));
        return 0;
    }

    return 1;
}

int messaging_subscribe(const char *channel_name)
{
    sk_atomic_begin();
    
    channel_t *c = channel_get(channel_name);

    if (c == NULL)
    {
        c = channel(channel_name);
        list_pushback(channels, c);
    }

    list_pushback(c->subscribers, thread_running()->process);
    
    sk_atomic_end();

    return 0;
}

int messaging_unsubscribe(const char *channel_name)
{
    sk_atomic_begin();
    
    channel_t *c = channel_get(channel_name);

    if (c != NULL)
    {
        list_remove(c->subscribers, thread_running()->process);
    }
    
    sk_atomic_end();

    return 0;
}
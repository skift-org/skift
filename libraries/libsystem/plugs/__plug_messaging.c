/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/messaging.h>
#include <libkernel/syscalls.h>
#include <libsystem/error.h>

int messaging_send(message_t *event)
{
    int r = __syscall(SYS_MESSAGING_SEND, (int)event, 0, 0, 0, 0);

    RETURN_AND_SET_ERROR(r, r, -1);
}

int messaging_broadcast(const char *channel, message_t *event)
{
    int r = __syscall(SYS_MESSAGING_BROADCAST, (int)channel, (int)event, 0, 0, 0);

    RETURN_AND_SET_ERROR(r, r, -1);
}

int messaging_request(message_t *request, message_t *result, int timeout)
{
    int r = __syscall(SYS_MESSAGING_REQUEST, (int)request, (int)result, timeout, 0, 0);

    RETURN_AND_SET_ERROR(r, r, -1);
}

int messaging_receive(message_t *message, bool wait)
{
    int r = __syscall(SYS_MESSAGING_RECEIVE, (int)message, (int)wait, 0, 0, 0);

    RETURN_AND_SET_ERROR(r, r, -1);
}

int messaging_respond(message_t *request, message_t *result)
{
    int r = __syscall(SYS_MESSAGING_RESPOND, (int)request, (int)result, 0, 0, 0);

    RETURN_AND_SET_ERROR(r, r, -1);
}

int messaging_subscribe(const char *channel)
{
    int r = __syscall(SYS_MESSAGING_SUBSCRIBE, (int)channel, 0, 0, 0, 0);

    RETURN_AND_SET_ERROR(r, r, -1);
}

int messaging_unsubscribe(const char *channel)
{
    int r = __syscall(SYS_MESSAGING_UNSUBSCRIBE, (int)channel, 0, 0, 0, 0);

    RETURN_AND_SET_ERROR(r, r, -1);
}
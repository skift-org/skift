#pragma once

#include <skift/runtime.h>

#include <hjert/syscalls.h>
#include <hjert/message.h>

DECL_SYSCALL5(messaging_send, int to, const char *name, void *payload, unsigned int size, unsigned int flags);
DECL_SYSCALL5(messaging_broadcast, const char *channel, const char *name, void *payload, unsigned int size, unsigned int flags);

DECL_SYSCALL2(messaging_receive, message_t *msg, int wait);
DECL_SYSCALL2(messaging_payload, void *buffer, unsigned int size);

DECL_SYSCALL1(messaging_subscribe, const char *channel);
DECL_SYSCALL1(messaging_unsubscribe, const char *channel);

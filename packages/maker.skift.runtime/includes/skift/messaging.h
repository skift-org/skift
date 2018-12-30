#pragma once

#include <skift/syscall.h>
#include "kernel/protocol.h"

DECL_SYSCALL5(sk_messaging_send, int to, const char * name, void * payload, unsigned int size, unsigned int flags);
DECL_SYSCALL5(sk_messaging_broadcast, const char * channel, const char * name, void * payload, unsigned int size, unsigned int flags);

DECL_SYSCALL1(sk_messaging_receive, message_t * msg);
DECL_SYSCALL2(sk_messaging_payload, void * buffer, unsigned int size);

DECL_SYSCALL1(sk_messaging_subscribe, const char * channel);
DECL_SYSCALL1(sk_messaging_unsubscribe, const char * channel);

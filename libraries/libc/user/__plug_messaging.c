#include <skift/messaging.h>

DEFN_SYSCALL5(messaging_send, SYS_MSG_SEND, int, const char *, void *, unsigned int, unsigned int);
DEFN_SYSCALL5(messaging_broadcast, SYS_MSG_BROADCAST, const char *, const char *, void *, unsigned int, unsigned int);

DEFN_SYSCALL2(messaging_receive, SYS_MSG_RECEIVE, message_t *, int);

DEFN_SYSCALL1(messaging_subscribe, SYS_MSG_SUBSCRIBE, const char *);
DEFN_SYSCALL1(messaging_unsubscribe, SYS_MSG_UNSUBSCRIBE, const char *);

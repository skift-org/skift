#include <skift/messaging.h>

DEFN_SYSCALL5(sk_messaging_send, SYS_MSG_SEND, int, const char *, void *, unsigned int, unsigned int);
DEFN_SYSCALL5(sk_messaging_broadcast, SYS_MSG_BROADCAST, const char *, const char *, void *, unsigned int, unsigned int);

DEFN_SYSCALL1(sk_messaging_receive, SYS_MSG_RECEIVE, message_t *);
DEFN_SYSCALL2(sk_messaging_payload, SYS_MSG_PAYLOAD, void *, unsigned int);

DEFN_SYSCALL1(sk_messaging_subscribe, SYS_MSG_SUBSCRIBE, const char *);
DEFN_SYSCALL1(sk_messaging_unsubscribe, SYS_MSG_UNSUBSCRIBE, const char *);

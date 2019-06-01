#include <skift/logger.h>
#include <skift/messaging.h>
#include <skift/iostream.h>

#include <hjert/platform/irq.h>
#include <hjert/devices/keyboard.h>

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    logger_log(LOG_INFO, "Starting keyboard device driver...");

    messaging_subscribe(IRQ_CHANNEL(1));

    message_t msg;
    while (messaging_receive(&msg, true))
    {
        printf("message %s\n", message_label(msg));
    }

    return 0;
}

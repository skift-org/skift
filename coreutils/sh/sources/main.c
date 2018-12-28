#include <stdio.h>
#include <string.h>

#include <skift/process.h>
#include <skift/messaging.h>
#include <skift/thread.h>

#include "kernel/protocol.h"

bool exited = false;

void readline(char* buffer, uint size)
{
    (void)size;
    buffer[0] = '\0';

    while(true)
    {
        message_t msg;
        sk_messaging_receive(&msg);

        if (strcmp(msg.name, KEYBOARD_KEYTYPED) == 0)
        {
            keyboard_event_t event;
            sk_messaging_payload(&event, sizeof(keyboard_event_t));

            if (event.c == '\n')
            {
                printf("\n");
                return;
            }
            else if (!(event.c == '\0' || event.c == '\t' || event.c == '\b'))
            {
                strapd(buffer, event.c);
                printf("%c", event.c);
            }
        }
    }
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    sk_messaging_subscribe(KEYBOARD_CHANNEL);

    while (!exited)
    {
        printf("\n\033[0;31m $ \033[0m");
        
        char buffer[128];
        readline(buffer, 128);
        int process = sk_process_exec(buffer, NULL);
        if (process)
        {
            sk_thread_waitproc(process);
        }
        else
        {
            printf("Command not found !\n");
        }
    }

    return 0;
}

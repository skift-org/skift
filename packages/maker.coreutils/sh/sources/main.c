#include <stdio.h>
#include <string.h>

#include <skift/process.h>
#include <skift/messaging.h>
#include <skift/thread.h>

#include "kernel/protocol.h"

#define PROMPT "\n\033[0;31m $ \033[0m"

bool exited = false;
void readline(char* buffer, uint size)
{
    sk_messaging_subscribe(KEYBOARD_CHANNEL);

    int i = 0;    
    buffer[i] = '\0';

    while(true)
    {
        message_t msg;
        sk_thread_wait_message(&msg);

        if (strcmp(msg.label, KEYBOARD_KEYTYPED) == 0)
        {
            keyboard_event_t event;
            sk_messaging_payload(&event, sizeof(keyboard_event_t));

            if (event.c == '\n')
            {
                printf("\n");
                break;
            }
            else if (event.c == '\b')
            {
                if (strlen(buffer) > 0)
                {
                    strbs(buffer);
                    printf("\b\033[2J");
                }
            }
            else if (!(event.c == '\0' || event.c == '\t'))
            {
                if (strlen(buffer) < size - 1)
                {
                    strnapd(buffer, event.c, size);
                    printf("%c", event.c);
                }
            }
        }
    }

    sk_messaging_unsubscribe(KEYBOARD_CHANNEL);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    while (!exited)
    {
        printf(PROMPT);
        
        char command[128];
        
        readline(command, 128);

        strleadtrim(command, ' ');

        if (strlen(command) != 0)
        {
            int process = sk_process_exec(command, NULL);

            if (!process)
            {
                char pathbuffer[144];
                snprintf(pathbuffer, 144, "/bin/%s", command);
                process = sk_process_exec(pathbuffer, NULL);
            }
            
            if (process)
                sk_thread_wait_process(process);
            else
                printf("Command '%s' not found !\n", command);
        } 
    }

    return 0;
}

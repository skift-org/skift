/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/messaging.h>
#include <skift/cstring.h>
#include <skift/iostream.h>

#include <hjert/devkit/keyboard.h>

#include "shell.h"

int shell_readline(shell_t* this)
{
    UNUSED(this);

    // FIXME: All user input should come from in_stream
    messaging_subscribe(KEYBOARD_CHANNEL);

    int i = 0;
    this->command_string[i] = '\0';

    while (true)
    {
        message_t msg;
        messaging_receive(&msg, 1);

        if (strcmp(msg.label, KEYBOARD_KEYTYPED) == 0)
        {
            keyboard_event_t event;
            messaging_payload(&event, sizeof(keyboard_event_t));

            if (event.c == '\n')
            {
                printf("\n");
                break;
            }
            else if (event.c == '\b')
            {
                if (strlen(this->command_string) > 0)
                {
                    strbs(this->command_string);
                    printf("\b\033[K");
                }
            }
            else if (!(event.c == '\0' || event.c == '\t'))
            {
                if (strlen(this->command_string) < MAX_COMMAND_LENGHT - 1)
                {
                    strnapd(this->command_string, event.c, MAX_COMMAND_LENGHT);
                    printf("%c", event.c);
                }
            }
        }
    }

    messaging_unsubscribe(KEYBOARD_CHANNEL);

    printf("\033[0m");

    return strlen(this->command_string);
}
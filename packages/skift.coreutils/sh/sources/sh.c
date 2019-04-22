/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>

#include <skift/iostream.h>
#include <skift/logger.h>
#include <skift/process.h>
#include <skift/messaging.h>
#include <skift/thread.h>

#include "kernel/protocol.h"
#include "kernel/limits.h"

#define PROMPT "\033[0;1;34m $ \033[0;1m"

static bool exited = false;

int shell_readline(char* buffer, uint size)
{
    sk_messaging_subscribe(KEYBOARD_CHANNEL);

    int i = 0;    
    buffer[i] = '\0';

    while(true)
    {
        message_t msg;
        sk_messaging_receive(&msg, 1);

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
                    printf("\b\033[K");
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

    printf("\033[0m");

    return strlen(buffer);
}

char** shell_split(char* command)
{
    strleadtrim(command, ' ');
    strtrailtrim(command, ' ');

    if (strlen(command) == 0) return NULL;

    int token_index = 0;

    char** tokens = malloc(MAX_PROCESS_ARGV * sizeof(char*));

    sk_log(LOG_DEBUG, "tokens list at %08x", tokens);

    memset(tokens, 0, MAX_PROCESS_ARGV * sizeof(char*));
    char* start = &command[0];

    for(size_t i = 0; i < strlen(command) + 1; i++)
    {
        char c = command[i];

        if (c == ' ' || c == '\0')
        {
            int buffer_len = &command[i] - start + 1;

            if (buffer_len > 1)
            {
                char* buffer = malloc(buffer_len);
                memcpy(buffer, start, buffer_len);
                buffer[buffer_len - 1] = '\0';

                tokens[token_index++] = buffer;
                printf("'%s'\n", buffer);
            }

            start = &command[i] + 1;
        }
    }

    tokens[token_index++] = NULL;
    
    return tokens;
}

int shell_eval(const char** command)
{
    
    int exitvalue = -1;

    int process = sk_process_exec(command[0], command);

    /* FIXME
    if (!process)
    {
        char pathbuffer[144];
        snprintf(pathbuffer, 144, "/bin/%s", command[0]);

        process = sk_process_exec(pathbuffer, command);
    }
    */
    
    if (process)
        sk_thread_wait_process(process, &exitvalue);
    else
        printf("Command '%s' not found !\n", command[0]);

    return exitvalue;
}

int main(int argc, char **argv)
{
    sk_logger_setlevel(LOG_ALL);

    // HACK: Disable line buffering
    out_stream->write_buffer = 0;
    out_stream->write_buffer = NULL;

    (void)argc;
    (void)argv;

    int exitvalue = 0;

    while (!exited)
    {
        if (exitvalue != 0)
        {
            printf("\033[0;1;31m%d", exitvalue);
        }

        printf(PROMPT);
        
        char** tokens;
        char command[128];
        
        if (shell_readline(command, 128) > 0 && (tokens = shell_split(command)) != NULL)
        {
            exitvalue = shell_eval((const char**)tokens);
            
            for(int i = 0; tokens[i] != NULL; i++)
            {
                sk_log(LOG_DEBUG, "%d '%s'", i, tokens[i]);
                // FIXME : cause page fault
                //free(tokens[i]);
            }
            
            //free(tokens);
        }
        else
        {
            printf("Empty command!\n");
        }


        printf("\033[0m");

    }

    return 0;
}

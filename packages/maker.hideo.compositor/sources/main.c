/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>

#include <skift/messaging.h>
#include <skift/process.h>
#include <skift/thread.h>
#include <skift/logger.h>
#include <skift/io.h>

#include <hideo.h>

void die(const char *msg)
{
    sk_log(LOG_FATAL, "Hideo die: %s", msg);
    sk_process_exit(-1);
}

void hideo_keydown_handler()
{
}

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    sk_logger_setlevel(LOG_ALL);

    // Setup the graphic context
    uint width, height = 0;
    sk_io_graphic_size(&width, &height);

    if (width < 120 || height < 120)
    {
        die("Invalid graphic context!");
    }

    // Create the hideo context
    hideo_context_t hideo = {
        .running = true,
    };

    // Enter the message loop
    sk_messaging_subscribe(KEYBOARD_CHANNEL);
    sk_messaging_subscribe(MOUSE_CHANNEL);

    while (hideo.running)
    {
        message_t msg;
        sk_messaging_receive(&msg);

        sk_log(LOG_DEBUG, "hideo msg: %s", msg.name);

        if (strcmp(msg.name, MOUSE_MOVE) == 0)
        {
        }
        else if (strcmp(msg.name, MOUSE_SCROLL) == 0)
        {
        }
        else if (strcmp(msg.name, MOUSE_BUTTONDOWN) == 0)
        {
        }
        else if (strcmp(msg.name, MOUSE_BUTTONUP) == 0)
        {
        }
        else if (strcmp(msg.name, KEYBOARD_KEYDOWN) == 0)
        {
        }
        else if (strcmp(msg.name, KEYBOARD_KEYUP) == 0)
        {
        }
        else if (strcmp(msg.name, KEYBOARD_KEYTYPED) == 0)
        {
        }
    }

    return 0;
}

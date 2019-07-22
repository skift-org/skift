/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/messaging.h>
#include <skift/logger.h>
#include <skift/process.h>

#include "compositor/server.h"

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    logger_setlevel(LOG_ALL);

    hideo_server_t* server = hideo_server();

    hideo_server_create_window(server, "Hello, world! (window 1)");
    hideo_server_create_window(server, "Hello, world! (window 2)");   
    hideo_server_create_window(server, "Hello, world! (window 3)");   
    hideo_server_create_window(server, "Hello, world! (window 4)");

    hideo_server_set_wallpaper(server, "/res/wallpaper/brand.png");

    hideo_server_blit_all(server);

    messaging_subscribe(MOUSE_CHANNEL);
    messaging_subscribe(KEYBOARD_CHANNEL);

    // Write the pid to a file for client to connect to:
    iostream_t* fpid = iostream_open("/run/hideo_pid", IOSTREAM_WRITE | IOSTREAM_CREATE);
    iostream_printf(fpid, "%d", process_this());
    iostream_close(fpid);

    while (hideo_server_running(server))
    {
        message_t message;
        messaging_receive(&message, true);

        logger_log(LOG_DEBUG, "-- v -- Handeling event '%s' started -- v --", message_label(message));

        if (message_is(message, MOUSE_MOVE))
        {
            hideo_server_mouse_move(server, message_payload_as(message, mouse_move_event_t));
        }
        else if (message_is(message, MOUSE_SCROLL))
        {
            hideo_server_mouse_scroll(server, message_payload_as(message, mouse_scroll_event_t));
        }
        else if (message_is(message, MOUSE_BUTTONUP))
        {
            hideo_server_mouse_btnup(server, message_payload_as(message, mouse_button_event_t));
        }
        else if (message_is(message, MOUSE_BUTTONDOWN))
        {
            hideo_server_mouse_btndown(server, message_payload_as(message, mouse_button_event_t));
        }
        else
        {
            logger_log(LOG_WARNING, "Unknow message %s!", message_label(message));
            return -1;
        }

        hideo_server_blit_all(server);

        logger_log(LOG_DEBUG, "-- ^ -- Handeling event '%s' endded -- ^ --", message_label(message));

    }

    object_release(server);

    return 0;
}
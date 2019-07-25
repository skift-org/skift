/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/iostream.h>
#include <skift/cstring.h>
#include <skift/error.h>
#include <skift/bitmap.h>
#include <skift/painter.h>
#include <skift/logger.h>
#include <skift/assert.h>
#include <skift/messaging.h>

#include <hideo/server.h>
#include <hideo/client.h>
#include <hjert/devices/framebuffer.h>

/* --- wm_client object ----------------------------------------------------- */

class(wm_client)
{
    int pid;
    list_t *windows;
} wm_client_t;

constructor(wm_client, int pid)
{
}

destructor(wm_client)
{
}

methode(void, wm_client, respond, int value)
{
}

/* --- wm_window object ----------------------------------------------------- */

class(wm_window)
{
    int id;

    point_t position;
    point_t size;

    int shm;
    bitmap_t* framebuffer;
} wm_window_t;

constructor(wm_window, wm_client_t *owner)
{
    static int wid = 0;

    this->id = wid++;

    return this;
}

destructor(wm_window)
{
    object_release(this->framebuffer);
}

/* --- The server logic ----------------------------------------------------- */

list_t *clients;
list_t *windows;

bitmap_t *framebuffer;
framebuffer_mode_info_t framebuffer_mode = {true, 800, 600};
iostream_t *framebuffer_device = NULL;
painter_t *framebuffer_painter;

bool wm_open_framebuffer(framebuffer_mode_info_t prefered_mode)
{
    framebuffer_device = iostream_open(FRAMEBUFFER_DEVICE, IOSTREAM_READ);

    if (framebuffer_device == NULL)
    {
        error_print("Failled to open " FRAMEBUFFER_DEVICE);

        return false;
    }

    framebuffer_mode_info_t current_mode;

    if (iostream_ioctl(framebuffer_device, FRAMEBUFFER_IOCTL_GET_MODE, &current_mode) < 0)
    {
        error_print("Ioctl(FRAMEBUFFER_IOCTL_GET_MODE) to " FRAMEBUFFER_DEVICE " failled");
        iostream_close(framebuffer_device);

        return false;
    }

    if (current_mode.enable &&
        current_mode.height >= prefered_mode.height &&
        current_mode.width >= prefered_mode.width)
    {
        framebuffer_mode = current_mode;
    }
    else
    {
        if (iostream_ioctl(framebuffer_device, FRAMEBUFFER_IOCTL_SET_MODE, &prefered_mode) < 0)
        {
            error_print("Ioctl(FRAMEBUFFER_IOCTL_SET_MODE) to " FRAMEBUFFER_DEVICE " failled");
            iostream_close(framebuffer_device);

            return false;
        }

        framebuffer_mode = prefered_mode;
    }

    framebuffer = bitmap(framebuffer_mode.width, framebuffer_mode.height);
    framebuffer_painter = painter(framebuffer);

    logger_log(LOG_FINE, "Frame buffer created");

    return true;
}

bool wm_handle_message()
{
    message_t incoming;

    messaging_receive(&incoming, true);

    logger_log(LOG_DEBUG, "Incoming %s", message_label(incoming));

    if (message_is(incoming, HIDEO_CLIENT_HELLO))
    {
        // Create the client object

        // Acknowledge
    }
    else if (message_is(incoming, HIDEO_CLIENT_GOODBYE))
    {
        // Closes all client windows

        // Detroy the client object

        // Acknowledge
    }
    else if (message_is(incoming, HIDEO_CLIENT_WINDOW_CREATE))
    {
        // Create the new window object

        // Respond with the window's id
    }
    else if (message_is(incoming, HIDEO_CLIENT_WINDOW_DESTROY))
    {
        // Destroy the window

        // Acknowledge
    }
    else if (message_is(incoming, HIDEO_CLIENT_WINDOW_SHOW))
    {
        // Show the window

        // Acknowledge

        // Repaint
    }
    else if (message_is(incoming, HIDEO_CLIENT_WINDOW_HIDE))
    {
        // Hide the window

        // Acknowledge

        // Repaint
    }
    else if (message_is(incoming, HIDEO_CLIENT_WINDOW_SET_POSITION))
    {
        // Set the window position

        // Acknowledge

        // Repaint
    }
    else if (message_is(incoming, HIDEO_CLIENT_WINDOW_SET_SIZE))
    {
        // Set the window size

        // Acknowledge

        // Repaint
    }
    else if (message_is(incoming, HIDEO_CLIENT_WINDOW_SET_ICON))
    {
        // Set the window icon

        // Acknowledge

        // Repaint
    }
    else if (message_is(incoming, HIDEO_CLIENT_WINDOW_SET_TITLE))
    {
        // Set the window title

        // Acknowledge

        // Repaint
    }

    return true;
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    assert(wm_open_framebuffer(framebuffer_mode))

        do
    {
        assert(wm_handle_message());
        iostream_ioctl(framebuffer_device, FRAMEBUFFER_IOCTL_BLIT, framebuffer->buffer);
    }
    while (true)
        ;

    return 0;
}
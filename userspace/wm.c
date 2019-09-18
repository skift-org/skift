/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/framebuffer.h>
#include <libdevice/keyboard.h>
#include <libdevice/keys.c>
#include <libdevice/mouse.h>
#include <libform/protocol.h>
#include <libgraphic/painter.h>
#include <libsystem/error.h>
#include <libsystem/eventloop.h>
#include <libsystem/iostream.h>
#include <libsystem/logger.h>

static eventloop_t *server_eventloop = NULL;

static iostream_t* server_framebuffer_device = NULL;
static bitmap_t *server_framebuffer = NULL;
static rectangle_t server_dirty_region;
static bool server_has_dirty_region = false;
static painter_t *server_painter = NULL;

/* --- Framebuffer ---------------------------------------------------------- */

iostream_t* server_framebuffer_open(int width, int height)
{
    iostream_t *framebuffer_device = iostream_open(FRAMEBUFFER_DEVICE, IOSTREAM_READ);

    if (framebuffer_device == NULL)
    {
        error_print("Failled to open " FRAMEBUFFER_DEVICE);
        
        return NULL;
    }

    framebuffer_mode_info_t mode_info = {true, width, height};

    if (iostream_call(framebuffer_device, FRAMEBUFFER_CALL_SET_MODE, &mode_info) < 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        
        return NULL;
    }

    return framebuffer_device;
}

void server_framebuffer_dirty(rectangle_t bound)
{
    // Make sure the bound is on the screen
    bound = rectangle_child(bitmap_bound(server_framebuffer), bound);

    if (bound.height != 0 && bound.width != 0)
    {
        if (server_has_dirty_region)
        {
            server_dirty_region = rectangle_merge(server_dirty_region, bound);
        }
        else
        {
            server_has_dirty_region = true;
            server_dirty_region = bound;
        }
    }
}

void server_framebuffer_repaint(void)
{
    if (server_has_dirty_region)
    {

        // TODO: repaint the screen.

        server_has_dirty_region = false;
    }
}

/* --- Keyboard event handlers ---------------------------------------------- */

void server_keypressed_handler(eventloop_t *eventloop, message_t *message, keyboard_event_t *payload)
{
    UNUSED(eventloop);
    UNUSED(message);

    logger_trace("KEYBOARD_KEYPRESSED: key: %s(%d) codepoint: %d", key_to_string(payload->key), payload->key, payload->codepoint);
}

void server_keyreleased_handler(eventloop_t *eventloop, message_t *message, keyboard_event_t *payload)
{
    UNUSED(eventloop);
    UNUSED(message);

    logger_trace("KEYBOARD_KEYRELEASED: key: %s(%d) codepoint: %d", key_to_string(payload->key), payload->key, payload->codepoint);
}

void server_keytyped_handler(eventloop_t *eventloop, message_t *message, keyboard_event_t *payload)
{
    UNUSED(eventloop);
    UNUSED(message);

    logger_trace("KEYBOARD_KEYTYPED: key: %s(%d) codepoint: %d", key_to_string(payload->key), payload->key, payload->codepoint);
}

/* --- Mouse event handlers ------------------------------------------------- */

void server_mousemove_handler(eventloop_t *eventloop, message_t *message, mouse_move_event_t *payload)
{
    UNUSED(eventloop);
    UNUSED(message);

    logger_trace("MOUSE_MOVE: offx=%d offy=%d", payload->offx, payload->offy);
}

void server_mousescroll_handler(eventloop_t *eventloop, message_t *message, mouse_scroll_event_t *payload)
{
    UNUSED(eventloop);
    UNUSED(message);

    logger_trace("MOUSE_SCROLL: off=%d", payload->off);
}

void server_mousepressed_handler(eventloop_t *eventloop, message_t *message, mouse_button_event_t *payload)
{
    UNUSED(eventloop);
    UNUSED(message);

    logger_trace("MOUSE_DOWN: button%d", payload->button);
}

void server_mousereleased_handler(eventloop_t *eventloop, message_t *message, mouse_button_event_t *payload)
{
    UNUSED(eventloop);
    UNUSED(message);

    logger_trace("MOUSE_PRESS: button%d", payload->button);
}

/* --- Server entry point --------------------------------------------------- */

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    server_framebuffer_device = server_framebuffer_open(800, 600);

    if( server_framebuffer_device == NULL)
    {
        logger_fatal("Failled to open the frame buffer device!");
        return -1;
    }

    server_framebuffer = bitmap(800, 600);
    server_painter = painter(server_framebuffer);

    painter_fill_rect(server_painter, bitmap_bound(server_framebuffer), COLOR_CORNFLOWERBLUE);

    server_eventloop = eventloop();

    // Subscribe to devices channels.
    eventloop_subscribe(server_eventloop, MOUSE_CHANNEL);
    eventloop_subscribe(server_eventloop, KEYBOARD_CHANNEL);
    eventloop_subscribe(server_eventloop, WINDOW_SERVER_CHANNEL);

    // Register keyboard events handlers.
    eventloop_register_handler(server_eventloop, KEYBOARD_KEYPRESSED, (eventloop_message_handler_t)server_keypressed_handler);
    eventloop_register_handler(server_eventloop, KEYBOARD_KEYRELEASED, (eventloop_message_handler_t)server_keyreleased_handler);
    eventloop_register_handler(server_eventloop, KEYBOARD_KEYTYPED, (eventloop_message_handler_t)server_keytyped_handler);

    // Register mouse events handlers.
    eventloop_register_handler(server_eventloop, MOUSE_MOVE, (eventloop_message_handler_t)server_mousemove_handler);
    eventloop_register_handler(server_eventloop, MOUSE_SCROLL, (eventloop_message_handler_t)server_mousescroll_handler);
    eventloop_register_handler(server_eventloop, MOUSE_BUTTONDOWN, (eventloop_message_handler_t)server_mousepressed_handler);
    eventloop_register_handler(server_eventloop, MOUSE_BUTTONUP, (eventloop_message_handler_t)server_mousereleased_handler);

    eventloop_run(server_eventloop);

    return 0;
}
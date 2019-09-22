/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/keyboard.h>
#include <libdevice/keys.c>
#include <libdevice/mouse.h>
#include <libform/protocol.h>
#include <libsystem/error.h>
#include <libsystem/eventloop.h>
#include <libsystem/iostream.h>
#include <libsystem/logger.h>
#include <libgraphic/framebuffer.h>

static framebuffer_t *server_framebuffer = NULL;
static eventloop_t *server_eventloop = NULL;

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

    server_framebuffer = framebuffer_open();

    if (server_framebuffer == NULL)
    {
        error_print("Failled to open the framebuffer.");
        return -1;
    }

    painter_clear(server_framebuffer->painter, COLOR_CORNFLOWERBLUE);

    framebuffer_blit(server_framebuffer);

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
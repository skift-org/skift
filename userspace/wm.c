/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/keyboard.h>
#include <libdevice/keys.c>
#include <libdevice/mouse.h>
#include <libform/protocol.h>
#include <libsystem/error.h>
#include <libsystem/messageloop.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libgraphic/framebuffer.h>

static framebuffer_t *server_framebuffer = NULL;

/* --- Keyboard event handlers ---------------------------------------------- */

void server_keypressed_handler(messageloop_t *messageloop, message_t *message, keyboard_event_t *payload)
{
    __unused(messageloop);
    __unused(message);

    logger_trace("KEYBOARD_KEYPRESSED: key: %s(%d) codepoint: %d", key_to_string(payload->key), payload->key, payload->codepoint);
}

void server_keyreleased_handler(messageloop_t *messageloop, message_t *message, keyboard_event_t *payload)
{
    __unused(messageloop);
    __unused(message);

    logger_trace("KEYBOARD_KEYRELEASED: key: %s(%d) codepoint: %d", key_to_string(payload->key), payload->key, payload->codepoint);
}

void server_keytyped_handler(messageloop_t *messageloop, message_t *message, keyboard_event_t *payload)
{
    __unused(messageloop);
    __unused(message);

    logger_trace("KEYBOARD_KEYTYPED: key: %s(%d) codepoint: %d", key_to_string(payload->key), payload->key, payload->codepoint);
}

/* --- Mouse event handlers ------------------------------------------------- */

void server_mousemove_handler(messageloop_t *messageloop, message_t *message, mouse_move_event_t *payload)
{
    __unused(messageloop);
    __unused(message);

    logger_trace("MOUSE_MOVE: offx=%d offy=%d", payload->offx, payload->offy);
}

void server_mousescroll_handler(messageloop_t *messageloop, message_t *message, mouse_scroll_event_t *payload)
{
    __unused(messageloop);
    __unused(message);

    logger_trace("MOUSE_SCROLL: off=%d", payload->off);
}

void server_mousepressed_handler(messageloop_t *messageloop, message_t *message, mouse_button_event_t *payload)
{
    __unused(messageloop);
    __unused(message);

    logger_trace("MOUSE_DOWN: button%d", payload->button);
}

void server_mousereleased_handler(messageloop_t *messageloop, message_t *message, mouse_button_event_t *payload)
{
    __unused(messageloop);
    __unused(message);

    logger_trace("MOUSE_PRESS: button%d", payload->button);
}

/* --- Server entry point --------------------------------------------------- */

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    server_framebuffer = framebuffer_open();

    if (server_framebuffer == NULL)
    {
        error_print("Failled to open the framebuffer.");
        return -1;
    }

    painter_clear(server_framebuffer->painter, COLOR_CORNFLOWERBLUE);

    framebuffer_blit(server_framebuffer);

    messageloop_init(argc, argv);

    // Subscribe to devices channels.
    messageloop_subscribe(MOUSE_CHANNEL);
    messageloop_subscribe(KEYBOARD_CHANNEL);
    messageloop_subscribe(WINDOW_SERVER_CHANNEL);

    // Register keyboard events handlers.
    messageloop_register_handler(KEYBOARD_KEYPRESSED, (messageloop_message_handler_t)server_keypressed_handler);
    messageloop_register_handler(KEYBOARD_KEYRELEASED, (messageloop_message_handler_t)server_keyreleased_handler);
    messageloop_register_handler(KEYBOARD_KEYTYPED, (messageloop_message_handler_t)server_keytyped_handler);

    // Register mouse events handlers.
    messageloop_register_handler(MOUSE_MOVE, (messageloop_message_handler_t)server_mousemove_handler);
    messageloop_register_handler(MOUSE_SCROLL, (messageloop_message_handler_t)server_mousescroll_handler);
    messageloop_register_handler(MOUSE_BUTTONDOWN, (messageloop_message_handler_t)server_mousepressed_handler);
    messageloop_register_handler(MOUSE_BUTTONUP, (messageloop_message_handler_t)server_mousereleased_handler);

    messageloop_run();

    messageloop_fini();

    return 0;
}
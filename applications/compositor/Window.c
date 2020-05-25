#include <libsystem/Assert.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>

#include "compositor/Client.h"
#include "compositor/Manager.h"
#include "compositor/Protocol.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

Window *window_create(
    int id,
    struct Client *client,
    Rectangle bound,
    Bitmap *frontbuffer,
    int frontbuffer_handle,
    Bitmap *backbuffer,
    int backbuffer_handle)
{
    Window *window = __create(Window);

    window->id = id;
    window->client = client;
    window->bound = bound;

    window->frontbuffer = frontbuffer;
    window->frontbuffer_handle = frontbuffer_handle;

    window->backbuffer = backbuffer;
    window->backbuffer_handle = backbuffer_handle;

    manager_register_window(window);

    return window;
}

void window_destroy(Window *window)
{
    manager_unregister_window(window);

    free(window);
}

Rectangle window_bound(Window *window)
{
    return window->bound;
}

Rectangle window_cursor_capture_bound(Window *window)
{
    return rectangle_expand(window_bound(window), INSETS(16));
}

void window_move(Window *window, Vec2i position)
{
    renderer_region_dirty(window_bound(window));

    window->bound.position = position;

    renderer_region_dirty(window_bound(window));
}

void window_resize(Window *window, Rectangle bound)
{
    renderer_region_dirty(window_bound(window));

    window->bound = bound;

    renderer_region_dirty(window_bound(window));
}

void window_send_event(Window *window, Event event)
{
    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_EVENT_WINDOW,
        .event_window = {
            .id = window->id,
            .event = event,
        },
    };

    client_send_message(window->client, message);
}

void window_handle_mouse_move(Window *window, Vec2i old_position, Vec2i position, MouseButton buttons)
{
    Event event = {
        .type = EVENT_MOUSE_MOVE,
        .mouse = {
            .position = position,
            .old_position = old_position,
            .button = MOUSE_NO_BUTTON,
            .buttons = buttons,
        },
    };

    window_send_event(window, event);
}

void window_handle_mouse_button(Window *window, MouseButton button, MouseButton old_buttons, MouseButton buttons, Vec2i position)
{
    bool was_button_pressed = old_buttons & button;
    bool is_button_pressed = buttons & button;

    if (is_button_pressed && !was_button_pressed)
    {
        Event event = {
            .type = EVENT_MOUSE_BUTTON_PRESS,
            .mouse = {
                .position = position,
                .old_position = position,
                .button = button,
                .buttons = buttons,
            },
        };

        window_send_event(window, event);
    }

    if (was_button_pressed && !is_button_pressed)
    {
        Event event = {
            .type = EVENT_MOUSE_BUTTON_RELEASE,
            .mouse = {
                .position = position,
                .old_position = position,
                .button = button,
                .buttons = buttons,
            },
        };

        window_send_event(window, event);
    }
}

void window_handle_mouse_buttons(
    Window *window,
    MouseButton old_buttons,
    MouseButton buttons,
    Vec2i position)
{
    window_handle_mouse_button(window, MOUSE_BUTTON_LEFT, old_buttons, buttons, position);
    window_handle_mouse_button(window, MOUSE_BUTTON_RIGHT, old_buttons, buttons, position);
    window_handle_mouse_button(window, MOUSE_BUTTON_MIDDLE, old_buttons, buttons, position);
}

void window_handle_double_click(Window *window, Vec2i position)
{
    Event event = {
        .type = EVENT_MOUSE_DOUBLE_CLICK,
        .mouse = {
            .position = position,
            .old_position = position,
            .button = MOUSE_BUTTON_LEFT,
            .buttons = MOUSE_BUTTON_LEFT,
        },
    };

    window_send_event(window, event);
}

void window_get_focus(Window *window)
{
    renderer_region_dirty(window_bound(window));
    window_send_event(window, (Event){.type = EVENT_GOT_FOCUS});
}

void window_lost_focus(Window *window)
{
    renderer_region_dirty(window_bound(window));
    window_send_event(window, (Event){.type = EVENT_LOST_FOCUS});
}

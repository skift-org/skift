#include <libsystem/cstring.h>

#include "Compositor/Client.h"
#include "Compositor/Manager.h"
#include "Compositor/Protocol.h"
#include "Compositor/Renderer.h"
#include "Compositor/Window.h"

Window *window_create(int id, Client *client, Rectangle bound, Bitmap *framebuffer)
{
    Window *window = __create(Window);

    window->id = id;
    window->client = client;
    window->bound = bound;
    window->framebuffer = framebuffer;

    manager_register_window(window);

    return window;
}

void window_destroy(Window *window)
{
    manager_unregister_window(window);

    free(window);
}

void window_send_event(Window *window, Event *event, size_t size)
{
    // FIXME: remove the need for malloc.

    CompositorWindowEvent *message = (CompositorWindowEvent *)malloc(sizeof(CompositorWindowEvent) + size);
    message->id = window->id;

    memcpy(&message->event, event, size);

    client_send_message(window->client, COMPOSITOR_MESSAGE_WINDOW_EVENT, message, sizeof(CompositorWindowEvent) + size);
    free(message);
}

Rectangle window_bound(Window *window)
{
    return window->bound;
}

void window_move(Window *window, Point position)
{
    renderer_region_dirty(window_bound(window));

    window->bound.position = position;

    renderer_region_dirty(window_bound(window));
}

void window_handle_mouse_move(Window *window, Point old_position, Point position, MouseButton buttons)
{
    bool mouse_was_in_window = rectangle_containe_point(window_bound(window), old_position);
    bool mouse_is_in_window = rectangle_containe_point(window_bound(window), position);

    if (mouse_is_in_window && !mouse_was_in_window)
    {
        window_send_event(window,
                          EVENT(
                              MouseEvent,
                              EVENT_MOUSE_ENTER,
                              position,
                              old_position,
                              MOUSE_NO_BUTTON,
                              buttons),
                          sizeof(MouseEvent));
    }
    else if (mouse_was_in_window && !mouse_is_in_window)
    {
        window_send_event(window,
                          EVENT(
                              MouseEvent,
                              EVENT_MOUSE_LEAVE,
                              position,
                              old_position,
                              MOUSE_NO_BUTTON,
                              buttons),
                          sizeof(MouseEvent));
    }

    window_send_event(window,
                      EVENT(
                          MouseEvent,
                          EVENT_MOUSE_MOVE,
                          position,
                          old_position,
                          MOUSE_NO_BUTTON,
                          buttons),
                      sizeof(MouseEvent));
}

void window_handle_mouse_button(Window *window, MouseButton button, MouseButton old_buttons, MouseButton buttons, Point position)
{
    bool was_button_pressed = button & old_buttons;
    bool is_button_pressed = button & buttons;

    if (is_button_pressed && !was_button_pressed)
    {
        window_send_event(window,
                          EVENT(
                              MouseEvent,
                              EVENT_MOUSE_BUTTON_PRESS,
                              position,
                              position,
                              button,
                              buttons),
                          sizeof(MouseEvent));
    }

    if (was_button_pressed && !is_button_pressed)
    {
        window_send_event(window,
                          EVENT(
                              MouseEvent,
                              EVENT_MOUSE_BUTTON_RELEASE,
                              position,
                              position,
                              button,
                              buttons),
                          sizeof(MouseEvent));
    }
}

void window_handle_mouse_buttons(
    Window *window,
    MouseButton old_buttons,
    MouseButton buttons,
    Point position)
{
    window_handle_mouse_button(window, MOUSE_BUTTON_LEFT, old_buttons, buttons, position);
    window_handle_mouse_button(window, MOUSE_BUTTON_RIGHT, old_buttons, buttons, position);
    window_handle_mouse_button(window, MOUSE_BUTTON_MIDDLE, old_buttons, buttons, position);
}

void window_get_focus(Window *window)
{
    renderer_region_dirty(window_bound(window));
    window_send_event(window, EVENT_NO_ARGS(EVENT_GOT_FOCUS), sizeof(Event));
}

void window_lost_focus(Window *window)
{
    renderer_region_dirty(window_bound(window));
    window_send_event(window, EVENT_NO_ARGS(EVENT_LOST_FOCUS), sizeof(Event));
}
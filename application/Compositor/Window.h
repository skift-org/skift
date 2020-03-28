#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Shape.h>
#include <libwidget/core/Cursor.h>
#include <libwidget/core/Event.h>

struct Client;

typedef struct Window
{
    int id;
    struct Client *client;
    Rectangle bound;
    CursorState cursor_state;

    Bitmap *framebuffer;
} Window;

Window *window_create(int id, struct Client *client, Rectangle bound, Bitmap *framebuffer);

void window_destroy(Window *window);

Rectangle window_bound(Window *window);

void window_move(Window *window, Point position);

void window_send_event(Window *window, Event *event, size_t size);

void window_handle_mouse_move(Window *window, Point old_position, Point position, MouseButton buttons);

void window_handle_mouse_buttons(Window *window, MouseButton old_buttons, MouseButton buttons, Point position);

void window_get_focus(Window *window);

void window_lost_focus(Window *window);
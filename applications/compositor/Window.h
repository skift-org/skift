#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Shape.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>

#include "compositor/Protocol.h"

struct Client;

struct Window
{
    int id;
    WindowFlag flags;
    struct Client *client;
    Rectangle bound;
    CursorState cursor_state;

    RefPtr<Bitmap> frontbuffer;
    RefPtr<Bitmap> backbuffer;
};

Window *window_create(
    int id,
    WindowFlag flags,
    struct Client *client,
    Rectangle bound,
    RefPtr<Bitmap> frontbuffer,
    RefPtr<Bitmap> backbuffer);

void window_destroy(Window *window);

Rectangle window_bound(Window *window);

Rectangle window_cursor_capture_bound(Window *window);

void window_move(Window *window, Vec2i position);

void window_resize(Window *window, Rectangle bound);

void window_handle_mouse_move(Window *window, Vec2i old_position, Vec2i position, MouseButton buttons);

void window_handle_mouse_buttons(Window *window, MouseButton old_buttons, MouseButton buttons, Vec2i position);

void window_handle_double_click(Window *window, Vec2i position);

void window_get_focus(Window *window);

void window_lost_focus(Window *window);

void window_send_event(Window *window, Event event);

#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libwidget/core/Cursor.h>
#include <libwidget/core/Event.h>
#include <libwidget/core/Widget.h>

typedef struct
{
    int id;
    char *title;

    bool focused;
    bool is_dragging;

    Rectangle bound;
    Widget *main_widget;
    CursorState cursor_state;

    int framebuffer_handle;
    Bitmap *framebuffer;
    Painter *painter;
} Window;

Window *window_create(const char *title, Rectangle bound);

void window_destroy(Window *window);

void window_dump(Window *window);

void window_handle_event(Window *window, Event *event);

void window_set_main_widget(Window *window, Widget *widget);

Rectangle window_bound_on_screen(Window *window);

Rectangle window_bound(Window *window);

Rectangle window_content_bound(Window *window);

void window_set_cursor(Window *window, CursorState state);

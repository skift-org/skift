#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>
#include <libwidget/Widget.h>

typedef struct Window Window;

Window *window_create(const char *title, int width, int height);

void window_destroy(Window *window);

void window_dump(Window *window);

void window_handle_event(Window *window, Event *event);

Rectangle window_bound_on_screen(Window *window);

Rectangle window_bound(Window *window);

Rectangle window_content_bound(Window *window);

void window_set_cursor(Window *window, CursorState state);

void window_set_background(Window *window, Color background);

void window_set_focused_widget(Window *window, Widget *widget);

int window_handle(Window *window);

int window_framebuffer_handle(Window *window);

Widget *window_root(Window *window);

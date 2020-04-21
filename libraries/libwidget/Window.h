#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>
#include <libwidget/Widget.h>

#define WINDOW_NONE (0)
#define WINDOW_BORDERLESS (1 << 0)
#define WINDOW_RESIZABLE (1 << 1)
#define WINDOW_ALWAYS_FOCUSED (1 << 2)

typedef unsigned int WindowFlag;

typedef void (*WindowDestroyCallback)(struct Window *window);

typedef struct Window
{
    int handle;

    bool focused;
    bool is_dragging;
    bool visible;

    WindowDestroyCallback destroy;

    Rectangle on_screen_bound;
    CursorState cursor_state;

    int framebuffer_handle;
    Bitmap *framebuffer;
    Painter *painter;

    List *dirty_rect;
    bool dirty_layout;

    Widget *header_container;
    Widget *root_container;
    Widget *focused_widget;

    Color background;

    WindowFlag flags;
} Window;

Window *window_create(
    const char *icon,
    const char *title,
    int width,
    int height,
    WindowFlag flags);

void window_initialize(
    Window *window,
    const char *icon,
    const char *title,
    int width,
    int height,
    WindowFlag flags);

void window_destroy(Window *window);

void window_show(Window *window);

void window_hide(Window *window);

bool window_is_visible(Window *window);

void window_paint(Window *window, Rectangle rectangle);

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

Widget *window_header(Window *window);

void window_schedule_update(Window *window, Rectangle rectangle);

void window_schedule_layout(Window *window);

bool window_is_focused(Window *window);

Color window_get_color(Window *window, ThemeColorRole role);

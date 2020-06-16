#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libsystem/utils/HashMap.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>
#include <libwidget/Widget.h>

#define WINDOW_NONE (0)
#define WINDOW_BORDERLESS (1 << 0)
#define WINDOW_RESIZABLE (1 << 1)
#define WINDOW_ALWAYS_FOCUSED (1 << 2)
#define WINDOW_POP_OVER (1 << 3)

typedef unsigned int WindowFlag;

typedef void (*WindowDestroyCallback)(struct Window *window);

typedef struct Window
{
    int handle;
    char *title;
    Icon *icon;
    WindowFlag flags;

    bool focused;
    bool visible;
    bool is_dragging;

    bool is_resizing;
    bool resize_vertical;
    bool resize_horizontal;
    Vec2i resize_begin;

    WindowDestroyCallback destroy;

    Rectangle on_screen_bound;
    CursorState cursor_state;

    Bitmap *frontbuffer;
    Painter *frontbuffer_painter;
    int frontbuffer_handle;

    Bitmap *backbuffer;
    Painter *backbuffer_painter;
    int backbuffer_handle;

    List *dirty_rect;
    bool dirty_layout;

    Widget *header_container;
    Widget *root_container;
    Widget *focused_widget;
    Widget *mouse_focused_widget;
    Widget *mouse_over_widget;
    HashMap *widget_by_id;
} Window;

Window *window_create(WindowFlag flags);

void window_initialize(Window *window, WindowFlag flags);

void window_destroy(Window *window);

void window_set_title(Window *window, const char *title);

void window_set_icon(Window *window, Icon *icon);

void window_set_size(Window *window, Vec2i size);

void window_set_position(Window *window, Vec2i position);

void window_set_bound(Window *window, Rectangle bound);

void window_show(Window *window);

void window_hide(Window *window);

bool window_is_visible(Window *window);

void window_paint(Window *window, Painter *painter, Rectangle rectangle);

void window_dump(Window *window);

void window_handle_event(Window *window, Event *event);

Rectangle window_bound_on_screen(Window *window);

Rectangle window_bound(Window *window);

Rectangle window_content_bound(Window *window);

void window_set_cursor(Window *window, CursorState state);

void window_set_focused_widget(Window *window, Widget *widget);

void window_widget_removed(Window *window, Widget *widget);

void window_register_widget_by_id(Window *window, const char *id, Widget *widget);

Widget *window_get_widget_by_id(Window *window, const char *id);

int window_handle(Window *window);

int window_frontbuffer_handle(Window *window);

int window_backbuffer_handle(Window *window);

Widget *window_root(Window *window);

Widget *window_header(Window *window);

void window_schedule_update(Window *window, Rectangle rectangle);

void window_schedule_layout(Window *window);

bool window_is_focused(Window *window);

Color window_get_color(Window *window, ThemeColorRole role);

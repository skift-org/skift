#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libsystem/utils/HashMap.h>
#include <libutils/Vector.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>
#include <libwidget/Widget.h>

#include "compositor/Protocol.h"

#define WINDOW_RESIZE_AREA 16
#define WINDOW_HEADER_AREA 36
#define WINDOW_CONTENT_PADDING 1

struct Window;

typedef void (*WindowDestroyCallback)(Window *window);

struct Window
{
    int handle;

    char *_title;
    RefPtr<Icon> _icon;
    Rectangle _bound;
    WindowFlag flags;
    WindowType _type;

    float _opacity;

    bool focused;
    bool visible;
    bool is_dragging;
    bool is_maximised;
    bool is_resizing;
    bool resize_vertical;
    bool resize_horizontal;
    Vec2i resize_begin;
    Rectangle previous_bound; // used for maximize
    WindowDestroyCallback destroy;

    CursorState cursor_state;

    RefPtr<Bitmap> frontbuffer;
    Painter frontbuffer_painter;

    RefPtr<Bitmap> backbuffer;
    Painter backbuffer_painter;

    List *dirty_rect;
    bool dirty_layout;

    EventHandler handlers[EventType::__COUNT];

    Widget *header_container;
    Widget *root_container;

    Widget *focused_widget;
    Widget *mouse_focused_widget;
    Widget *mouse_over_widget;
    HashMap *widget_by_id;

public:
    void title(const char *title);

    void icon(RefPtr<Icon> icon);

    Rectangle bound_on_screen() { return _bound; }

    Rectangle bound() { return _bound.moved({0, 0}); }

    Rectangle content_bound()
    {
        Rectangle result = bound();

        if (!(flags & WINDOW_BORDERLESS))
        {
            result = result.shrinked(Insets(WINDOW_HEADER_AREA, WINDOW_CONTENT_PADDING, WINDOW_CONTENT_PADDING));
        }

        return result;
    }

    int x() { return position().x(); }
    int y() { return position().y(); }
    int width() { return size().x(); }
    int height() { return size().y(); }

    void opacity(float value) { _opacity = value; }

    Vec2i position() { return bound_on_screen().position(); }

    void position(Vec2i position) { bound(bound_on_screen().moved(position)); }

    Vec2i size() { return bound().size(); }

    void size(Vec2i size) { bound(bound_on_screen().resized(size)); }

    void bound(Rectangle bound);

    void type(WindowType type);

    WindowType type() { return _type; }

    Widget *root() { return root_container; }

    Widget *header() { return header_container; }

    void on(EventType event, EventHandler handler);

    void show();

    void hide();
};

Window *window_create(WindowFlag flags);

void window_initialize(Window *window, WindowFlag flags);

void window_destroy(Window *window);

bool window_is_visible(Window *window);

void window_paint(Window *window, Painter &painter, Rectangle rectangle);

void window_event(Window *window, Event *event);

void window_set_cursor(Window *window, CursorState state);

void window_set_focused_widget(Window *window, Widget *widget);

void window_widget_removed(Window *window, Widget *widget);

void window_register_widget_by_id(Window *window, const char *id, Widget *widget);

Widget *window_get_widget_by_id(Window *window, const char *id);

int window_handle(Window *window);

int window_frontbuffer_handle(Window *window);

int window_backbuffer_handle(Window *window);

void window_schedule_update(Window *window, Rectangle rectangle);

void window_schedule_layout(Window *window);

bool window_is_focused(Window *window);

Color window_get_color(Window *window, ThemeColorRole role);

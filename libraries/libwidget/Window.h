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

struct Window
{
    int handle;

    char *_title;
    RefPtr<Icon> _icon;
    Rectangle _bound;
    WindowFlag _flags;
    WindowType _type;

    float _opacity;

    bool focused = false;
    bool visible = false;
    bool is_dragging = false;
    bool is_maximised = false;
    bool is_resizing = false;
    bool resize_vertical = false;
    bool resize_horizontal = false;
    Vec2i resize_begin;
    Rectangle previous_bound; // used for maximize

    CursorState cursor_state;

    RefPtr<Bitmap> frontbuffer;
    OwnPtr<Painter> frontbuffer_painter;

    RefPtr<Bitmap> backbuffer;
    OwnPtr<Painter> backbuffer_painter;

    List *dirty_rect;
    bool dirty_layout;

    EventHandler handlers[EventType::__COUNT];

    Widget *header_container;
    Widget *root_container;

    Widget *focused_widget = nullptr;
    Widget *mouse_focused_widget = nullptr;
    Widget *mouse_over_widget = nullptr;
    HashMap *widget_by_id = nullptr;

public:
    void title(const char *title);

    void icon(RefPtr<Icon> icon);

    int x() { return position().x(); }
    int y() { return position().y(); }
    int width() { return size().x(); }
    int height() { return size().y(); }

    Vec2i position() { return bound_on_screen().position(); }
    void position(Vec2i position) { bound(bound_on_screen().moved(position)); }

    Vec2i size() { return bound().size(); }
    void size(Vec2i size) { bound(bound_on_screen().resized(size)); }

    Rectangle bound() { return _bound.moved({0, 0}); }
    void bound(Rectangle bound);

    Rectangle bound_on_screen() { return _bound; }

    Rectangle content_bound()
    {
        if (_flags & WINDOW_BORDERLESS)
        {
            return bound();
        }
        else
        {
            return bound().shrinked(Insets(WINDOW_HEADER_AREA, WINDOW_CONTENT_PADDING, WINDOW_CONTENT_PADDING));
        }
    }

    void opacity(float value) { _opacity = value; }
    float opacity() { return _opacity; }

    WindowType type() { return _type; }
    void type(WindowType type) { _type = type; }

    Widget *root() { return root_container; }
    Widget *header() { return header_container; }

    Window(WindowFlag flags);
    virtual ~Window();

    void on(EventType event, EventHandler handler);

    void show();

    void hide();
};

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

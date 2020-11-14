#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libsystem/eventloop/Invoker.h>
#include <libutils/HashMap.h>
#include <libutils/Vector.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>
#include <libwidget/Widget.h>

#include "compositor/Protocol.h"

#define WINDOW_RESIZE_AREA 16
#define WINDOW_HEADER_AREA 36
#define WINDOW_CONTENT_PADDING 1

struct Window
{
    int _handle;

    String _title;
    RefPtr<Icon> _icon;
    Rectangle _bound;
    WindowFlag _flags;
    WindowType _type;

    float _opacity;

    bool _focused = false;
    bool _visible = false;
    bool is_dragging = false;
    bool is_maximised = false;
    bool is_resizing = false;
    bool resize_vertical = false;
    bool resize_horizontal = false;
    Vec2i resize_begin;
    Rectangle previous_bound;

    CursorState cursor_state;

    RefPtr<Bitmap> frontbuffer;
    OwnPtr<Painter> frontbuffer_painter;

    RefPtr<Bitmap> backbuffer;
    OwnPtr<Painter> backbuffer_painter;

    Vector<Rectangle> _dirty_rects{};
    bool dirty_layout;

    EventHandler handlers[EventType::__COUNT];

    Widget *header_container;
    Widget *root_container;

    Widget *focused_widget = nullptr;
    Widget *mouse_focused_widget = nullptr;
    Widget *mouse_over_widget = nullptr;
    HashMap<String, Widget *> widget_by_id{};

    OwnPtr<Invoker> _repaint_invoker;

    OwnPtr<Invoker> _relayout_invoker;

public:
    int handle() { return this->_handle; }
    int frontbuffer_handle() { return frontbuffer->handle(); }
    int backbuffer_handle() { return backbuffer->handle(); }

    void title(String title);
    void icon(RefPtr<Icon> icon);

    int x() { return position().x(); }
    int y() { return position().y(); }
    int width() { return size().x(); }
    int height() { return size().y(); }

    Vec2i position() { return bound_on_screen().position(); }
    void position(Vec2i position) { bound(bound_on_screen().moved(position)); }

    Vec2i size() { return bound().size(); }
    void size(Vec2i size);

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

    bool visible() { return _visible; }
    bool focused()
    {
        if (_flags & WINDOW_ALWAYS_FOCUSED)
        {
            return true;
        }
        else
        {
            return _focused;
        }
    }

    WindowType type() { return _type; }
    void type(WindowType type) { _type = type; }

    Color color(ThemeColorRole role);

    Widget *root() { return root_container; }
    Widget *header() { return header_container; }

    Window(WindowFlag flags);
    virtual ~Window();

    void on(EventType event, EventHandler handler);

    void show();

    void hide();

    void dispatch_event(Event *event);

    virtual void repaint(Painter &painter, Rectangle rectangle);

    void repaint_dirty();

    void relayout();

    void should_repaint(Rectangle rectangle);

    void should_relayout();

    template <typename WidgetType, typename CallbackType>
    void with_widget(String name, CallbackType callback)
    {
        if (widget_by_id.has_key(name))
        {
            auto widget = dynamic_cast<WidgetType *>(widget_by_id[name]);

            if (widget)
            {
                callback(widget);
            }
        }
    }

    void cursor(CursorState state);

    void focus_widget(Widget *widget);

    void widget_removed(Widget *widget);

    void register_widget_by_id(String id, Widget *widget);
};

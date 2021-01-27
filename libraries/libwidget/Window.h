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
#define WINDOW_CONTENT_PADDING 1

class Window
{
private:
    int _handle;

    String _title = "Window";
    RefPtr<Icon> _icon;
    Recti _bound{250, 250};
    WindowFlag _flags;
    WindowType _type = WINDOW_TYPE_REGULAR;

    float _opacity;

    bool _focused = false;
    bool _visible = false;

    bool _is_maximised = false;
    bool _is_resizing = false;

    bool _resize_vertical = false;
    bool _resize_horizontal = false;

    Vec2i _resize_begin;
    Recti _previous_bound;

    CursorState cursor_state = CURSOR_DEFAULT;

    RefPtr<Bitmap> frontbuffer;
    OwnPtr<Painter> frontbuffer_painter;

    RefPtr<Bitmap> backbuffer;
    OwnPtr<Painter> backbuffer_painter;

    Vector<Recti> _dirty_rects{};
    bool _dirty_layout;

    EventHandler _handlers[EventType::__COUNT];

    Widget *_root;

    Widget *_keyboard_focus = nullptr;
    Widget *_mouse_focus = nullptr;
    Widget *_mouse_over = nullptr;

    HashMap<String, Widget *> _widget_by_id{};

    OwnPtr<Invoker> _repaint_invoker;
    OwnPtr<Invoker> _relayout_invoker;

public:
    int handle() { return this->_handle; }

    int frontbuffer_handle() const { return frontbuffer->handle(); }

    Vec2i frontbuffer_size() const { return frontbuffer->size(); }

    int backbuffer_handle() const { return backbuffer->handle(); }

    Vec2i backbuffer_size() const { return backbuffer->size(); }

    void title(String title);
    String title() { return _title; }

    WindowFlag flags() { return _flags; }

    void icon(RefPtr<Icon> icon);
    RefPtr<Icon> icon() { return _icon; }

    int x() { return position().x(); }
    int y() { return position().y(); }
    int width() { return size().x(); }
    int height() { return size().y(); }

    Vec2i position() { return bound_on_screen().position(); }
    void position(Vec2i position) { bound(bound_on_screen().moved(position)); }

    Vec2i size() { return bound().size(); }
    void size(Vec2i size);

    Recti bound() { return _bound.moved({0, 0}); }
    void bound(Recti bound);

    Recti bound_on_screen() { return _bound; }

    Recti content_bound()
    {
        if (_flags & WINDOW_BORDERLESS)
        {
            return bound();
        }
        else
        {
            return bound().shrinked(WINDOW_CONTENT_PADDING);
        }
    }

    void opacity(float value) { _opacity = value; }

    float opacity() { return _opacity; }

    bool visible() { return _visible; }

    bool focused()
    {
        return (_flags & WINDOW_ALWAYS_FOCUSED) || _focused;
    }

    bool maximised()
    {
        return _is_maximised;
    }

    void toggle_maximise();

    WindowType type() { return _type; }

    void type(WindowType type) { _type = type; }

    Color color(ThemeColorRole role);

    Widget *root() { return _root; }

    Window(WindowFlag flags);

    virtual ~Window();

    void show();

    void hide();

    template <typename WidgetType, typename CallbackType>
    void with_widget(String name, CallbackType callback)
    {
        if (_widget_by_id.has_key(name))
        {
            auto widget = dynamic_cast<WidgetType *>(_widget_by_id[name]);

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

    void change_framebuffer_if_needed();

    Border resize_bound_containe(Vec2i position);

    void begin_resize(Vec2i mouse_position);

    void do_resize(Vec2i mouse_position);

    void end_resize();

    Widget *child_at(Vec2i position);

    /* --- Focus ------------------------------------------------------------ */

    bool has_keyboard_focus(Widget *widget);

    /* --- Layout ----------------------------------------------------------- */

    void relayout();

    void should_relayout();

    /* --- Render ----------------------------------------------------------- */

    virtual void repaint(Painter &painter, Recti rectangle);

    void repaint_dirty();

    void should_repaint(Recti rectangle);

    /* --- Events ----------------------------------------------------------- */

    void on(EventType event, EventHandler handler);

    void dispatch_event(Event *event);
};

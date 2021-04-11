#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libsystem/eventloop/Invoker.h>
#include <libutils/HashMap.h>
#include <libutils/Vector.h>
#include <libwidget/Component.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>

#include "compositor/Protocol.h"

#define WINDOW_RESIZE_AREA 16

namespace Widget
{

class Window
{
private:
    int _handle = -1;

    String _title = "Window";
    RefPtr<Graphic::Icon> _icon;
    Math::Recti _bound{250, 250};
    WindowFlag _flags;
    WindowType _type = WINDOW_TYPE_REGULAR;

    float _opacity;

    bool _focused = false;
    bool _visible = false;

    bool _is_maximised = false;
    bool _is_resizing = false;

    bool _resize_vertical = false;
    bool _resize_horizontal = false;

    Math::Vec2i _resize_begin;
    Math::Recti _previous_bound;

    CursorState cursor_state = CURSOR_DEFAULT;

    RefPtr<Graphic::Bitmap> frontbuffer;
    OwnPtr<Graphic::Painter> frontbuffer_painter;

    RefPtr<Graphic::Bitmap> backbuffer;
    OwnPtr<Graphic::Painter> backbuffer_painter;

    Vector<Math::Recti> _dirty_rects{};
    bool _dirty_layout;

    EventHandler _handlers[EventType::__COUNT];

    Component *_root;

    Component *_keyboard_focus = nullptr;
    Component *_mouse_focus = nullptr;
    Component *_mouse_over = nullptr;

    HashMap<String, Component *> _widget_by_id{};

    OwnPtr<Invoker> _repaint_invoker;
    OwnPtr<Invoker> _relayout_invoker;

public:
    int handle() { return this->_handle; }

    int frontbuffer_handle() const { return frontbuffer->handle(); }

    Math::Vec2i frontbuffer_size() const { return frontbuffer->size(); }

    int backbuffer_handle() const { return backbuffer->handle(); }

    Math::Vec2i backbuffer_size() const { return backbuffer->size(); }

    void title(String title) { _title = title; }

    String title() { return _title; }

    WindowFlag flags() { return _flags; }

    void icon(RefPtr<Graphic::Icon> icon)
    {
        if (icon)
        {
            _icon = icon;
        }
    }

    RefPtr<Graphic::Icon> icon() { return _icon; }

    void opacity(float value) { _opacity = value; }

    float opacity() { return _opacity; }

    bool visible() { return _visible; }

    bool focused()
    {
        return (_flags & WINDOW_ALWAYS_FOCUSED) || _focused;
    }

    bool maximised() { return _is_maximised; }

    void toggle_maximise();

    WindowType type() { return _type; }

    void type(WindowType type) { _type = type; }

    Graphic::Color color(ThemeColorRole role);

    Window(WindowFlag flags);

    virtual ~Window();

    void show();

    void hide();

    void cursor(CursorState state);

    /* --- Geometry --------------------------------------------------------- */

    Math::Vec2i position() { return bound_on_screen().position(); }

    void position(Math::Vec2i position) { bound(bound_on_screen().moved(position)); }

    Math::Vec2i size() { return bound().size(); }

    void size(Math::Vec2i size) { bound(bound_on_screen().resized(size)); }

    void resize_to_content();

    Math::Recti bound() { return _bound.moved({0, 0}); }

    void bound(Math::Recti bound);

    Math::Recti bound_on_screen() { return _bound; }

    void change_framebuffer_if_needed();

    Math::Border resize_bound_containe(Math::Vec2i position);

    void begin_resize(Math::Vec2i mouse_position);

    void do_resize(Math::Vec2i mouse_position);

    void end_resize();

    /* --- Childs ----------------------------------------------------------- */

    Component *root() { return _root; }

    void focus_widget(Component *widget);

    void widget_removed(Component *widget);

    void register_widget_by_id(String id, Component *widget);

    Component *child_at(Math::Vec2i position);

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

    /* --- Focus ------------------------------------------------------------ */

    bool has_keyboard_focus(Component *widget);

    /* --- Layout ----------------------------------------------------------- */

    void relayout();

    void should_relayout();

    /* --- Render ----------------------------------------------------------- */

    virtual void repaint(Graphic::Painter &painter, Math::Recti rectangle);

    void repaint_dirty();

    void should_repaint(Math::Recti rectangle);

    /* --- Events ----------------------------------------------------------- */

    void on(EventType event, EventHandler handler);

    void dispatch_event(Event *event);

    void handle_event(Event *Event);

    void handle_got_focus(Event *event);

    void handle_lost_focus(Event *event);

    void handle_window_closing(Event *event);

    void handle_mouse_move(Event *event);

    void handle_mouse_scroll(Event *event);

    void handle_mouse_button_press(Event *event);

    void handle_mouse_button_release(Event *event);

    void handle_mouse_double_click(Event *event);

    void handle_keyboard_key_typed(Event *event);

    void handle_keyboard_key_press(Event *event);

    void handle_keyboard_key_release(Event *event);
};

} // namespace Widget

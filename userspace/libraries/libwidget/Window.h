#pragma once

#include <libasync/Invoker.h>
#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libutils/HashMap.h>
#include <libutils/Vector.h>
#include <libwidget/Cursor.h>
#include <libwidget/Element.h>
#include <libwidget/Event.h>

#include "compositor/Protocol.h"

#define WINDOW_RESIZE_AREA 16

namespace Widget
{

class Window
{
private:
    int _handle = -1;

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

    RefPtr<Element> _root;

    Element *_keyboard_focus = nullptr;
    Element *_mouse_focus = nullptr;
    Element *_mouse_over = nullptr;

    OwnPtr<Async::Invoker> _repaint_invoker;
    OwnPtr<Async::Invoker> _relayout_invoker;

public:
    int handle() { return this->_handle; }

    int frontbuffer_handle() const { return frontbuffer->handle(); }

    Math::Vec2i frontbuffer_size() const { return frontbuffer->size(); }

    int backbuffer_handle() const { return backbuffer->handle(); }

    Math::Vec2i backbuffer_size() const { return backbuffer->size(); }

    WindowFlag flags() { return _flags; }

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

    RefPtr<Element> root() { return _root; }

    void focus_widget(Element *widget);

    void widget_removed(Element *widget);

    Element *child_at(Math::Vec2i position);

    /* --- Focus ------------------------------------------------------------ */

    bool has_keyboard_focus(Element *widget);

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

static inline Window *window(WindowFlag flags, Math::Vec2i size, RefPtr<Element> root)
{
    auto window = new Window(flags);
    window->size(size);
    window->root()->add(root);
    return window;
}

static inline Window *window(WindowFlag flags, RefPtr<Element> root)
{
    auto window = new Window(flags);
    window->root()->add(root);
    window->resize_to_content();
    return window;
}

static inline Window *window(RefPtr<Element> root)
{
    return window(WINDOW_NONE, root);
}

} // namespace Widget

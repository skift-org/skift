#include <assert.h>

#include <libsystem/system/Memory.h>
#include <libwidget/Application.h>
#include <libwidget/Container.h>
#include <libwidget/Event.h>
#include <libwidget/Screen.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>

namespace Widget
{

void Window::toggle_maximise()
{
    if (_is_maximised == true)
    {
        _is_maximised = false;
        bound(_previous_bound);
    }
    else
    {
        _is_maximised = true;
        _previous_bound = _bound;
        Math::Recti new_size = Screen::bound();
        new_size = Math::Recti(0, 51, new_size.width(), new_size.height() - 51);

        bound(new_size);
    }
}

void Window::bound(Math::Recti new_bound)
{
    auto have_same_size = _bound.size() == new_bound.size();
    auto have_same_position = _bound.position() == new_bound.position();

    _bound = new_bound;

    if (!_visible)
    {
        return;
    }

    if (!have_same_size)
    {
        change_framebuffer_if_needed();

        should_relayout();
        should_repaint(bound());
    }
    else if (!have_same_position)
    {
        Application::move_window(this, _bound.position());
    }

    if (!have_same_size)
    {
        Event size_changed = {};
        size_changed.type = Event::WINDOW_RESIZED;
        dispatch_event(&size_changed);
    }
}

Window::Window(WindowFlag flags)
{
    static int window_handle_counter = 0;
    _handle = window_handle_counter++;

    _flags = flags;

    frontbuffer = Graphic::Bitmap::create_shared(250, 250).unwrap();
    frontbuffer_painter = own<Graphic::Painter>(frontbuffer);

    backbuffer = Graphic::Bitmap::create_shared(250, 250).unwrap();
    backbuffer_painter = own<Graphic::Painter>(backbuffer);

    _root = make<Container>();
    _root->window(this);

    _keyboard_focus = _root.naked();

    _repaint_invoker = own<Async::Invoker>([this]() { repaint_dirty(); });
    _relayout_invoker = own<Async::Invoker>([this]() { relayout(); });

    Application::add_window(this);
    position({96, 72});
}

Window::~Window()
{
    if (_visible)
    {
        hide();
    }

    Application::remove_window(this);
}

void Window::repaint(Graphic::Painter &painter, Math::Recti rectangle)
{
    if (_flags & WINDOW_TRANSPARENT || _flags & WINDOW_ACRYLIC)
    {
        painter.clear(rectangle, color(THEME_BACKGROUND).with_alpha(_opacity));
    }
    else
    {
        painter.clear(rectangle, color(THEME_BACKGROUND));
    }

    painter.push();
    painter.clip(rectangle);

    if (root())
    {
        root()->repaint(painter, rectangle);
    }

    if (!(_flags & WINDOW_BORDERLESS))
    {
        painter.draw_rectangle_rounded(bound(), 6, 1, color(THEME_BORDER));
    }

    painter.pop();
}

void Window::repaint_dirty()
{
    // FIXME: find a better way to schedule update after layout.

    if (_dirty_layout)
    {
        relayout();
    }

    Math::Recti repaited_regions = Math::Recti::empty();
    Graphic::Painter &painter = *backbuffer_painter;

    _dirty_rects.foreach ([&](Math::Recti &rect) {
        repaint(painter, rect);

        if (repaited_regions.is_empty())
        {
            repaited_regions = rect;
        }
        else
        {
            repaited_regions = rect.merged_with(repaited_regions);
        }

        return Iteration::CONTINUE;
    });

    _dirty_rects.clear();

    frontbuffer->copy_from(*backbuffer, repaited_regions);

    swap(frontbuffer, backbuffer);
    swap(frontbuffer_painter, backbuffer_painter);

    Application::flip_window(this, repaited_regions);
}

void Window::relayout()
{
    root()->container(bound());
    root()->relayout();

    _dirty_layout = false;
}

void Window::should_repaint(Math::Recti rectangle)
{
    if (!_visible)
    {
        return;
    }

    if (_dirty_rects.empty())
    {
        _repaint_invoker->invoke_later();
    }

    for (size_t i = 0; i < _dirty_rects.count(); i++)
    {
        if (_dirty_rects[i].colide_with(rectangle))
        {
            _dirty_rects[i] = _dirty_rects[i].merged_with(rectangle);
            return;
        }
    }

    _dirty_rects.push_back(rectangle);
}

void Window::should_relayout()
{
    if (_dirty_layout || !_visible)
    {
        return;
    }

    _dirty_layout = true;

    _relayout_invoker->invoke_later();
}

void Window::change_framebuffer_if_needed()
{
    if (bound().width() > frontbuffer->width() ||
        bound().height() > frontbuffer->height() ||
        bound().area() < frontbuffer->bound().area() * 0.75)
    {
        frontbuffer = Graphic::Bitmap::create_shared(bound().width(), bound().height()).unwrap();
        frontbuffer_painter = own<Graphic::Painter>(frontbuffer);

        backbuffer = Graphic::Bitmap::create_shared(bound().width(), bound().height()).unwrap();
        backbuffer_painter = own<Graphic::Painter>(backbuffer);
    }
}

void Window::show()
{
    if (_visible)
    {
        return;
    }

    _visible = true;

    change_framebuffer_if_needed();

    relayout();
    repaint(*backbuffer_painter, bound());

    frontbuffer->copy_from(*backbuffer, bound());

    swap(frontbuffer, backbuffer);
    swap(frontbuffer_painter, backbuffer_painter);

    Application::show_window(this);
}

void Window::hide()
{
    if (!_visible)
    {
        return;
    }

    _relayout_invoker->cancel();
    _repaint_invoker->cancel();

    _visible = false;
    Application::hide_window(this);
}

Math::Border Window::resize_bound_containe(Math::Vec2i position)
{
    Math::Recti resize_bound = bound().expended(Insets(WINDOW_RESIZE_AREA));
    return resize_bound.contains(Insets(WINDOW_RESIZE_AREA), position);
}

void Window::begin_resize(Math::Vec2i mouse_position)
{
    _is_resizing = true;

    Math::Border borders = resize_bound_containe(mouse_position);

    _resize_horizontal = borders & (Math::Border::LEFT | Math::Border::RIGHT);
    _resize_vertical = borders & (Math::Border::TOP | Math::Border::BOTTOM);

    Math::Vec2i resize_region_begin = position();

    if (borders & Math::Border::TOP)
    {
        resize_region_begin += size().extract_y();
    }

    if (borders & Math::Border::LEFT)
    {
        resize_region_begin += size().extract_x();
    }

    _resize_begin = resize_region_begin;
}

void Window::do_resize(Math::Vec2i mouse_position)
{
    Math::Recti new_bound = Math::Recti::from_two_point(
        _resize_begin,
        position() + mouse_position);

    if (!_resize_horizontal)
    {
        new_bound = new_bound
                        .moved({position().x(), new_bound.y()})
                        .with_width(bound().width());
    }

    if (!_resize_vertical)
    {
        new_bound = new_bound
                        .moved({new_bound.x(), position().y()})
                        .with_height(bound().height());
    }

    Math::Vec2i content_size = root()->compute_size();

    new_bound = new_bound.with_width(MAX(new_bound.width(), content_size.x()));
    new_bound = new_bound.with_height(MAX(new_bound.height(), content_size.y()));

    bound(new_bound);

    Event resize_event = {};
    resize_event.type = Event::WINDOW_RESIZED;
    dispatch_event(&resize_event);
}

void Window::resize_to_content()
{
    Math::Vec2i content_size = root()->compute_size();
    bound({position(), content_size});
}

void Window::end_resize()
{
    _is_resizing = false;
}

Element *Window::child_at(Math::Vec2i position)
{
    return root()->at(position);
}

void Window::on(EventType event, EventHandler handler)
{
    Assert::lower_than(event, EventType::__COUNT);
    _handlers[event] = move(handler);
}

void Window::dispatch_event(Event *event)
{
    Assert::not_null(event);

    if (is_mouse_event(event))
    {
        // Convert mouse events to the local coordinate system
        event->mouse.position = event->mouse.position - _bound.position();
        event->mouse.old_position = event->mouse.old_position - _bound.position();
    }

    if (_handlers[event->type])
    {
        event->accepted = true;
        _handlers[event->type](event);
    }

    if (event->accepted)
    {
        return;
    }

    handle_event(event);
}

void Window::handle_event(Event *event)
{
    switch (event->type)
    {
    case Event::GOT_FOCUS:
        handle_got_focus(event);
        break;

    case Event::LOST_FOCUS:
        handle_lost_focus(event);
        break;

    case Event::WINDOW_CLOSING:
        handle_window_closing(event);
        break;

    case Event::MOUSE_MOVE:
        handle_mouse_move(event);
        break;

    case Event::MOUSE_SCROLL:
        handle_mouse_scroll(event);
        break;

    case Event::MOUSE_BUTTON_PRESS:
        handle_mouse_button_press(event);
        break;

    case Event::MOUSE_BUTTON_RELEASE:
        handle_mouse_button_release(event);
        break;

    case Event::MOUSE_DOUBLE_CLICK:
        handle_mouse_double_click(event);
        break;

    case Event::KEYBOARD_KEY_TYPED:
        handle_keyboard_key_typed(event);
        break;

    case Event::KEYBOARD_KEY_PRESS:
        handle_keyboard_key_press(event);
        break;

    case Event::KEYBOARD_KEY_RELEASE:
        handle_keyboard_key_release(event);
        break;

    default:
        break;
    }
}

void Window::handle_got_focus(Event *)
{
    _focused = true;
    should_repaint(bound());
}

void Window::handle_lost_focus(Event *event)
{
    if (_flags & WINDOW_AUTO_CLOSE)
    {
        hide();
    }
    else
    {
        _focused = false;
        should_repaint(bound());

        if (_mouse_focus)
        {
            Element *old_focus = _mouse_focus;
            _mouse_focus = nullptr;

            Event e = *event;
            e.type = Event::MOUSE_BUTTON_RELEASE;
            old_focus->dispatch_event(&e);
        }

        if (_mouse_over)
        {
            Event e = *event;
            e.type = Event::MOUSE_LEAVE;
            _mouse_over->dispatch_event(&e);

            _mouse_over = nullptr;
        }
    }
}

void Window::handle_window_closing(Event *)
{
    hide();
}

void Window::handle_mouse_move(Event *event)
{
    Math::Border borders = resize_bound_containe(event->mouse.position);

    if (_is_resizing && !_is_maximised)
    {
        do_resize(event->mouse.position);
    }
    else if (!_mouse_focus && borders && (_flags & WINDOW_RESIZABLE) && !_is_maximised)
    {
        if ((borders & Math::Border::TOP) && (borders & Math::Border::LEFT))
        {
            cursor(CURSOR_RESIZEHV);
        }
        else if ((borders & Math::Border::BOTTOM) && (borders & Math::Border::RIGHT))
        {
            cursor(CURSOR_RESIZEHV);
        }
        else if ((borders & Math::Border::TOP) && (borders & Math::Border::RIGHT))
        {
            cursor(CURSOR_RESIZEVH);
        }
        else if ((borders & Math::Border::BOTTOM) && (borders & Math::Border::LEFT))
        {
            cursor(CURSOR_RESIZEVH);
        }
        else if (borders & (Math::Border::TOP | Math::Border::BOTTOM))
        {
            cursor(CURSOR_RESIZEV);
        }
        else if (borders & (Math::Border::LEFT | Math::Border::RIGHT))
        {
            cursor(CURSOR_RESIZEH);
        }
    }
    else
    {
        auto result = child_at(event->mouse.position);

        if (_mouse_focus)
        {
            cursor(_mouse_focus->cursor());
        }
        else if (result)
        {
            cursor(result->cursor());
        }
        else
        {
            cursor(CURSOR_DEFAULT);
        }

        if (_mouse_over != result)
        {

            if (_mouse_over)
            {
                Event mouse_leave = *event;
                mouse_leave.type = Event::MOUSE_LEAVE;

                _mouse_over->dispatch_event(&mouse_leave);
            }

            if (result)
            {
                Event mouse_enter = *event;
                mouse_enter.type = Event::MOUSE_ENTER;

                result->dispatch_event(&mouse_enter);
            }

            _mouse_over = result;
        }

        if (_mouse_focus)
        {
            auto position_in_window = _mouse_focus->position_in_window();

            Event event_copy = *event;

            event_copy.mouse.position -= position_in_window;
            event_copy.mouse.old_position -= position_in_window;

            _mouse_focus->dispatch_event(&event_copy);

            event->accepted = event_copy.accepted;
        }
    }
}

void Window::handle_mouse_scroll(Event *event)
{
    auto result = child_at(event->mouse.position);

    if (!result)
        return;

    auto window_position = result->position_in_window();

    Event event_copy = *event;

    event_copy.mouse.position -= window_position;
    event_copy.mouse.old_position -= window_position;

    result->dispatch_event(&event_copy);

    event->accepted = event_copy.accepted;
}

void Window::handle_mouse_button_press(Event *event)
{
    if (root()->bound().contains(event->mouse.position))
    {
        auto result = child_at(event->mouse.position);

        if (result)
        {
            auto window_position = result->position_in_window();

            Event event_copy = *event;

            event_copy.mouse.position -= window_position;
            event_copy.mouse.old_position -= window_position;

            _mouse_focus = result;
            result->dispatch_event(&event_copy);

            event->accepted = event_copy.accepted;
        }
    }

    if (!event->accepted && !(_flags & WINDOW_BORDERLESS))
    {
        if ((_flags & WINDOW_RESIZABLE) &&
            !event->accepted &&
            !_is_resizing &&
            !_is_maximised &&
            resize_bound_containe(event->mouse.position))
        {
            begin_resize(event->mouse.position);
        }
    }
}

void Window::handle_mouse_button_release(Event *event)
{
    if (_mouse_focus)
    {
        Element *old_focus = _mouse_focus;
        _mouse_focus = nullptr;

        old_focus->dispatch_event(event);
    }

    if (_is_resizing)
    {
        end_resize();
    }
}

void Window::handle_mouse_double_click(Event *event)
{
    if (root()->bound().contains(event->mouse.position))
    {
        auto result = child_at(event->mouse.position);

        if (result)
        {
            result->dispatch_event(event);
        }
    }
}

void Window::handle_keyboard_key_typed(Event *event)
{
    if (_keyboard_focus)
    {
        _keyboard_focus->dispatch_event(event);
    }
}

void Window::handle_keyboard_key_press(Event *event)
{
    if (_keyboard_focus)
    {
        _keyboard_focus->dispatch_event(event);
    }
}

void Window::handle_keyboard_key_release(Event *event)
{
    if (_keyboard_focus)
    {
        _keyboard_focus->dispatch_event(event);
    }
}

void Window::cursor(CursorState state)
{
    if (_is_resizing)
    {
        return;
    }

    if (cursor_state != state)
    {
        Application::window_change_cursor(this, state);
        cursor_state = state;
    }
}

void Window::focus_widget(Element *widget)
{
    _keyboard_focus = widget;
}

bool Window::has_keyboard_focus(Element *widget)
{
    return _keyboard_focus == widget;
}

void Window::widget_removed(Element *widget)
{
    if (_keyboard_focus == widget)
    {
        _keyboard_focus = nullptr;
    }

    if (_mouse_over == widget)
    {
        _mouse_over = nullptr;
    }

    if (_mouse_focus == widget)
    {
        _mouse_focus = nullptr;
    }
}

Graphic::Color Window::color(ThemeColorRole role)
{
    if (!focused())
    {
        if (role == THEME_FOREGROUND)
        {
            role = THEME_FOREGROUND_INACTIVE;
        }

        if (role == THEME_SELECTION)
        {
            role = THEME_SELECTION_INACTIVE;
        }

        if (role == THEME_ACCENT)
        {
            role = THEME_ACCENT_INACTIVE;
        }
    }

    return theme_get_color(role);
}

} // namespace Widget

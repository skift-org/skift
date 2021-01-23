#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/io/Stream.h>
#include <libsystem/system/Memory.h>

#include <libwidget/Application.h>
#include <libwidget/Container.h>
#include <libwidget/Event.h>
#include <libwidget/Screen.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>

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
        Recti new_size = Screen::bound();
        new_size = Recti(0, 38, new_size.width(), new_size.height() - 38);

        bound(new_size);
    }
}

Window::Window(WindowFlag flags)
{
    static int window_handle_counter = 0;
    _handle = window_handle_counter++;

    _icon = Icon::get("application");
    _flags = flags;

    frontbuffer = Bitmap::create_shared(250, 250).take_value();
    frontbuffer_painter = own<Painter>(frontbuffer);

    backbuffer = Bitmap::create_shared(250, 250).take_value();
    backbuffer_painter = own<Painter>(backbuffer);

    _root = new Container(nullptr);
    _root->window(this);

    _keyboard_focus = _root;

    _repaint_invoker = own<Invoker>([this]() {
        repaint_dirty();
    });

    _relayout_invoker = own<Invoker>([this]() {
        relayout();
    });

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

    delete _root;
}

void Window::repaint(Painter &painter, Recti rectangle)
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

    if (!content_bound().contains(rectangle))
    {
        if (!(_flags & WINDOW_BORDERLESS))
        {
            painter.draw_rectangle(bound(), color(THEME_ACCENT));
        }
    }

    painter.pop();
}

void Window::repaint_dirty()
{
    // FIXME: find a better way to schedule update after layout.

    if (dirty_layout)
    {
        relayout();
    }

    Recti repaited_regions = Recti::empty();
    Painter &painter = *backbuffer_painter;

    _dirty_rects.foreach ([&](Recti &rect) {
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
    root()->container(content_bound());
    root()->relayout();

    dirty_layout = false;
}

void Window::should_repaint(Recti rectangle)
{
    if (!_visible)
        return;

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
    if (dirty_layout || !_visible)
    {
        return;
    }

    dirty_layout = true;

    _relayout_invoker->invoke_later();
}

static void window_change_framebuffer_if_needed(Window *window)
{
    if (window->bound().width() > window->frontbuffer->width() ||
        window->bound().height() > window->frontbuffer->height() ||
        window->bound().area() < window->frontbuffer->bound().area() * 0.75)
    {
        window->frontbuffer = Bitmap::create_shared(window->width(), window->height()).take_value();
        window->frontbuffer_painter = own<Painter>(window->frontbuffer);

        window->backbuffer = Bitmap::create_shared(window->width(), window->height()).take_value();
        window->backbuffer_painter = own<Painter>(window->backbuffer);
    }
}

void Window::size(Vec2i size)
{
    bound(bound_on_screen().resized(size));

    Event size_changed = {};
    size_changed.type = Event::WINDOW_RESIZED;
    dispatch_event(&size_changed);
}

void Window::show()
{
    if (_visible)
    {
        return;
    }

    _visible = true;

    window_change_framebuffer_if_needed(this);

    relayout();
    repaint(*backbuffer_painter, bound());

    frontbuffer->copy_from(*backbuffer, bound());

    swap(frontbuffer, backbuffer);
    swap(frontbuffer_painter, backbuffer_painter);

    Application::show_window(this);
}

void Window::hide()
{
    _relayout_invoker->cancel();
    _repaint_invoker->cancel();

    if (!_visible)
    {
        return;
    }

    _visible = false;
    Application::hide_window(this);
}

Border window_resize_bound_containe(Window *window, Vec2i position)
{
    Recti resize_bound = window->bound().expended(Insets(WINDOW_RESIZE_AREA));
    return resize_bound.contains(Insets(WINDOW_RESIZE_AREA), position);
}

void window_begin_resize(Window *window, Vec2i mouse_position)
{
    window->_is_resizing = true;

    Border borders = window_resize_bound_containe(window, mouse_position);

    window->_resize_horizontal = borders & (Border::LEFT | Border::RIGHT);
    window->_resize_vertical = borders & (Border::TOP | Border::BOTTOM);

    Vec2i resize_region_begin = window->position();

    if (borders & Border::TOP)
    {
        resize_region_begin += Vec2i(0, window->height());
    }

    if (borders & Border::LEFT)
    {
        resize_region_begin += Vec2i(window->width(), 0);
    }

    window->_resize_begin = resize_region_begin;
}

void window_do_resize(Window *window, Vec2i mouse_position)
{
    Recti new_bound = Recti::from_two_point(
        window->_resize_begin,
        window->position() + mouse_position);

    if (!window->_resize_horizontal)
    {
        new_bound = new_bound
                        .moved({window->x(), new_bound.y()})
                        .with_width(window->width());
    }

    if (!window->_resize_vertical)
    {
        new_bound = new_bound
                        .moved({new_bound.x(), window->y()})
                        .with_height(window->height());
    }

    Vec2i content_size = window->root()->compute_size();

    new_bound = new_bound.with_width(MAX(new_bound.width(), content_size.x() + WINDOW_CONTENT_PADDING * 2));
    new_bound = new_bound.with_height(MAX(new_bound.height(), content_size.y() + WINDOW_CONTENT_PADDING));

    window->bound(new_bound);

    Event resize_event = {};
    resize_event.type = Event::WINDOW_RESIZED;
    window->dispatch_event(&resize_event);
}

void window_end_resize(Window *window)
{
    window->_is_resizing = false;
}

Widget *window_child_at(Window *window, Vec2i position)
{
    return window->root()->child_at(position);
}

void Window::dispatch_event(Event *event)
{
    assert(event);

    if (is_mouse_event(event))
    {
        event->mouse.position = event->mouse.position - _bound.position();
        event->mouse.old_position = event->mouse.old_position - _bound.position();
    }

    if (handlers[event->type])
    {
        event->accepted = true;
        handlers[event->type](event);
    }

    if (event->accepted)
    {
        return;
    }

    switch (event->type)
    {
    case Event::GOT_FOCUS:
    {
        _focused = true;

        should_repaint(bound());
    }
    break;

    case Event::LOST_FOCUS:
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
                Widget *old_focus = _mouse_focus;
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
    break;

    case Event::WINDOW_CLOSING:
    {
        hide();
    }
    break;

    case Event::MOUSE_MOVE:
    {
        Border borders = window_resize_bound_containe(this, event->mouse.position);

        if (_is_resizing && !_is_maximised)
        {
            window_do_resize(this, event->mouse.position);
        }
        else if (!_mouse_focus && borders && (_flags & WINDOW_RESIZABLE) && !_is_maximised)
        {
            if ((borders & Border::TOP) && (borders & Border::LEFT))
            {
                cursor(CURSOR_RESIZEHV);
            }
            else if ((borders & Border::BOTTOM) && (borders & Border::RIGHT))
            {
                cursor(CURSOR_RESIZEHV);
            }
            else if ((borders & Border::TOP) && (borders & Border::RIGHT))
            {
                cursor(CURSOR_RESIZEVH);
            }
            else if ((borders & Border::BOTTOM) && (borders & Border::LEFT))
            {
                cursor(CURSOR_RESIZEVH);
            }
            else if (borders & (Border::TOP | Border::BOTTOM))
            {
                cursor(CURSOR_RESIZEV);
            }
            else if (borders & (Border::LEFT | Border::RIGHT))
            {
                cursor(CURSOR_RESIZEH);
            }
        }
        else
        {
            auto result = window_child_at(this, event->mouse.position);

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
                auto window_position = result->position_in_window();

                Event event_copy = *event;

                event_copy.mouse.position -= window_position;
                event_copy.mouse.old_position -= window_position;

                _mouse_focus->dispatch_event(&event_copy);

                event->accepted = event_copy.accepted;
            }
        }

        break;
    }

    case Event::MOUSE_BUTTON_PRESS:
    {
        if (root()->bound().contains(event->mouse.position))
        {

            auto result = window_child_at(this, event->mouse.position);

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
                window_resize_bound_containe(this, event->mouse.position))
            {
                window_begin_resize(this, event->mouse.position);
            }
        }

        break;
    }

    case Event::MOUSE_BUTTON_RELEASE:
    {
        if (_mouse_focus)
        {
            Widget *old_focus = _mouse_focus;
            _mouse_focus = nullptr;

            old_focus->dispatch_event(event);
        }

        if (_is_resizing)
        {
            window_end_resize(this);
        }

        break;
    }

    case Event::MOUSE_DOUBLE_CLICK:
    {
        if (root()->bound().contains(event->mouse.position))
        {
            auto result = window_child_at(this, event->mouse.position);

            if (result)
            {
                result->dispatch_event(event);
            }
        }

        break;
    }

    case Event::KEYBOARD_KEY_TYPED:
    case Event::KEYBOARD_KEY_PRESS:
    case Event::KEYBOARD_KEY_RELEASE:
    {
        if (_keyboard_focus)
        {
            _keyboard_focus->dispatch_event(event);
        }
        break;
    }
    case Event::DISPLAY_SIZE_CHANGED:
    {
        break;
    }

    default:
        break;
    }
}

void Window::on(EventType event, EventHandler handler)
{
    assert(event < EventType::__COUNT);
    handlers[event] = move(handler);
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

void Window::focus_widget(Widget *widget)
{
    _keyboard_focus = widget;
}

void Window::widget_removed(Widget *widget)
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

    widget_by_id.remove_value(widget);
}

void Window::register_widget_by_id(String id, Widget *widget)
{
    widget_by_id[id] = widget;
}

Color Window::color(ThemeColorRole role)
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

void Window::title(String title)
{
    _title = title;
}

void Window::icon(RefPtr<Icon> icon)
{
    if (icon)
    {
        _icon = icon;
    }
}

void Window::bound(Recti new_bound)
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
        window_change_framebuffer_if_needed(this);

        should_relayout();
        should_repaint(bound());
    }
    else if (!have_same_position)
    {
        Application::move_window(this, _bound.position());
    }
}

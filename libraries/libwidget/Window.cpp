#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/io/Stream.h>
#include <libsystem/system/Memory.h>
#include <libwidget/Application.h>
#include <libwidget/Event.h>
#include <libwidget/Screen.h>
#include <libwidget/Theme.h>
#include <libwidget/Widgets.h>
#include <libwidget/Window.h>

Rectangle window_header_bound(Window *window);

void window_populate_header(Window *window)
{
    window->header_container->clear_children();

    window->header()->layout(HFLOW(4));
    window->header()->insets(Insets(6, 6));

    new Button(
        window->header(),
        BUTTON_TEXT,
        window->_icon,
        window->_title);

    auto spacer = new Container(window->header());
    spacer->attributes(LAYOUT_FILL);

    if (window->_flags & WINDOW_RESIZABLE)
    {
        Widget *button_minimize = new Button(window->header(), BUTTON_TEXT, Icon::get("window-minimize"));
        button_minimize->insets(Insets(3));

        Widget *button_maximize = new Button(window->header(), BUTTON_TEXT, Icon::get("window-maximize"));
        button_maximize->insets(Insets(3));
        button_maximize->on(Event::ACTION, [window](auto) {
            Event maximise_event = {};
            maximise_event.type = Event::WINDOW_MAXIMIZING;
            window->dispatch_event(&maximise_event);
        });
    }

    Widget *close_button = new Button(window->header(), BUTTON_TEXT, Icon::get("window-close"));
    close_button->insets(Insets(3));

    close_button->on(Event::ACTION, [window](auto) {
        Event close_event = {};
        close_event.type = Event::WINDOW_CLOSING;
        window->dispatch_event(&close_event);
    });
}

Window::Window(WindowFlag flags)
{
    static int window_handle_counter = 0;
    _handle = window_handle_counter++;
    _title = strdup("Window");
    _icon = Icon::get("application");
    _type = WINDOW_TYPE_REGULAR;
    _flags = flags;
    is_maximised = false;
    _focused = false;
    cursor_state = CURSOR_DEFAULT;

    frontbuffer = Bitmap::create_shared(250, 250).take_value();
    frontbuffer_painter = own<Painter>(frontbuffer);

    backbuffer = Bitmap::create_shared(250, 250).take_value();
    backbuffer_painter = own<Painter>(backbuffer);

    _bound = Rectangle(250, 250);

    header_container = new Container(nullptr);
    header_container->window(this);

    window_populate_header(this);

    root_container = new Container(nullptr);
    root_container->window(this);

    focused_widget = root_container;

    _repaint_invoker = own<Invoker>([this]() {
        repaint_dirty();
    });

    _relayout_invoker = own<Invoker>([this]() {
        relayout();
    });

    application_add_window(this);
    position(Vec2i(96, 72));
}

Window::~Window()
{
    if (_visible)
    {
        hide();
    }

    application_remove_window(this);

    delete root_container;
    delete header_container;
}

void Window::repaint(Painter &painter, Rectangle rectangle)
{

    if (_flags & WINDOW_TRANSPARENT)
    {
        painter.clear_rectangle(rectangle, color(THEME_BACKGROUND).with_alpha(_opacity));
    }
    else
    {
        painter.clear_rectangle(rectangle, color(THEME_BACKGROUND));
    }

    painter.push();
    painter.clip(rectangle);

    if (content_bound().contains(rectangle))
    {
        if (root())
        {
            root()->repaint(painter, rectangle);
        }
    }
    else
    {
        if (root())
        {
            root()->repaint(painter, rectangle);
        }

        if (!(_flags & WINDOW_BORDERLESS))
        {
            if (header())
            {
                header()->repaint(painter, rectangle);
            }

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

    Rectangle repaited_regions = Rectangle::empty();
    Painter &painter = *backbuffer_painter;

    _dirty_rects.foreach ([&](Rectangle &rect) {
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

    application_flip_window(this, repaited_regions);
}

void Window::relayout()
{
    header()->bound(window_header_bound(this));
    header()->relayout();

    root()->bound(content_bound());
    root()->relayout();

    dirty_layout = false;
}

void Window::should_repaint(Rectangle rectangle)
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
        return;

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
        return;

    _visible = true;

    window_change_framebuffer_if_needed(this);

    relayout();
    repaint(*backbuffer_painter, bound());

    application_show_window(this);
}

void Window::hide()
{
    _relayout_invoker->cancel();
    _repaint_invoker->cancel();

    if (!_visible)
        return;

    _visible = false;
    application_hide_window(this);
}

Rectangle window_header_bound(Window *window)
{
    return window->bound().take_top(WINDOW_HEADER_AREA);
}

RectangleBorder window_resize_bound_containe(Window *window, Vec2i position)
{
    Rectangle resize_bound = window->bound().expended(Insets(WINDOW_RESIZE_AREA));
    return resize_bound.contains(Insets(WINDOW_RESIZE_AREA), position);
}

void window_begin_resize(Window *window, Vec2i mouse_position)
{
    window->is_resizing = true;

    RectangleBorder borders = window_resize_bound_containe(window, mouse_position);

    window->resize_horizontal = borders & (RectangleBorder::LEFT | RectangleBorder::RIGHT);
    window->resize_vertical = borders & (RectangleBorder::TOP | RectangleBorder::BOTTOM);

    Vec2i resize_region_begin = window->position();

    if (borders & RectangleBorder::TOP)
    {
        resize_region_begin += Vec2i(0, window->height());
    }

    if (borders & RectangleBorder::LEFT)
    {
        resize_region_begin += Vec2i(window->width(), 0);
    }

    window->resize_begin = resize_region_begin;
}

void window_do_resize(Window *window, Vec2i mouse_position)
{
    Rectangle new_bound = Rectangle::from_two_point(
        window->resize_begin,
        window->position() + mouse_position);

    if (!window->resize_horizontal)
    {
        new_bound = new_bound
                        .moved({window->x(), new_bound.y()})
                        .with_width(window->width());
    }

    if (!window->resize_vertical)
    {
        new_bound = new_bound
                        .moved({new_bound.x(), window->y()})
                        .with_height(window->height());
    }

    Vec2i content_size = window->root()->compute_size();

    new_bound = new_bound.with_width(MAX(new_bound.width(), MAX(window->header()->compute_size().x(), content_size.x()) + WINDOW_CONTENT_PADDING * 2));
    new_bound = new_bound.with_height(MAX(new_bound.height(), WINDOW_HEADER_AREA + content_size.y() + WINDOW_CONTENT_PADDING));

    window->bound(new_bound);

    Event resize_event = {};
    resize_event.type = Event::WINDOW_RESIZED;
    window->dispatch_event(&resize_event);
}

void window_end_resize(Window *window)
{
    window->is_resizing = false;
}

Widget *window_child_at(Window *window, Vec2i position)
{
    if (window->root()->bound().contains(position))
    {
        return window->root()->child_at(position);
    }

    if (window->header()->bound().contains(position))
    {
        return window->header()->child_at(position);
    }

    return nullptr;
}

void window_update(Window *window); // for maximize

void Window::dispatch_event(Event *event)
{
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
        _focused = false;
        should_repaint(bound());

        Event mouse_leave = *event;
        mouse_leave.type = Event::MOUSE_LEAVE;

        if (mouse_over_widget)
        {
            mouse_over_widget->dispatch_event(&mouse_leave);
        }

        if (_type == WINDOW_TYPE_POPOVER)
        {
            hide();
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
        RectangleBorder borders = window_resize_bound_containe(this, event->mouse.position);

        if (is_dragging && !is_maximised)
        {
            Vec2i offset = event->mouse.position - event->mouse.old_position;
            _bound = _bound.offset(offset);
            application_move_window(this, _bound.position());
        }
        else if (is_resizing && !is_maximised)
        {
            window_do_resize(this, event->mouse.position);
        }
        else if (borders && (_flags & WINDOW_RESIZABLE) && !is_maximised)
        {
            if ((borders & RectangleBorder::TOP) && (borders & RectangleBorder::LEFT))
            {
                cursor(CURSOR_RESIZEHV);
            }
            else if ((borders & RectangleBorder::BOTTOM) && (borders & RectangleBorder::RIGHT))
            {
                cursor(CURSOR_RESIZEHV);
            }
            else if ((borders & RectangleBorder::TOP) && (borders & RectangleBorder::RIGHT))
            {
                cursor(CURSOR_RESIZEVH);
            }
            else if ((borders & RectangleBorder::BOTTOM) && (borders & RectangleBorder::LEFT))
            {
                cursor(CURSOR_RESIZEVH);
            }
            else if (borders & (RectangleBorder::TOP | RectangleBorder::BOTTOM))
            {
                cursor(CURSOR_RESIZEV);
            }
            else if (borders & (RectangleBorder::LEFT | RectangleBorder::RIGHT))
            {
                cursor(CURSOR_RESIZEH);
            }
        }
        else
        {
            // FIXME: Set the cursor based on the focused widget.
            cursor(CURSOR_DEFAULT);

            Widget *result = window_child_at(this, event->mouse.position);

            if (mouse_over_widget != result)
            {
                Event mouse_leave = *event;
                mouse_leave.type = Event::MOUSE_LEAVE;

                if (mouse_over_widget)
                {
                    mouse_over_widget->dispatch_event(&mouse_leave);
                }

                Event mouse_enter = *event;
                mouse_enter.type = Event::MOUSE_ENTER;

                if (result)
                {
                    result->dispatch_event(&mouse_enter);
                }

                mouse_over_widget = result;
            }

            if (mouse_focused_widget)
            {
                mouse_focused_widget->dispatch_event(event);
            }
        }

        break;
    }

    case Event::MOUSE_BUTTON_PRESS:
    {
        if (root()->bound().contains(event->mouse.position))
        {
            Widget *widget = window_child_at(this, event->mouse.position);

            if (widget)
            {
                mouse_focused_widget = widget;
                widget->dispatch_event(event);
            }
        }

        if (!event->accepted && !(_flags & WINDOW_BORDERLESS))
        {
            if (!event->accepted && header()->bound().contains(event->mouse.position))
            {
                Widget *widget = header()->child_at(event->mouse.position);

                if (widget)
                {
                    widget->dispatch_event(event);
                }
            }

            if (!event->accepted &&
                !is_dragging &&
                !is_maximised &&
                event->mouse.button == MOUSE_BUTTON_LEFT &&
                window_header_bound(this).contains(event->mouse.position))
            {
                is_dragging = true;
                cursor(CURSOR_MOVE);
            }

            if ((_flags & WINDOW_RESIZABLE) &&
                !event->accepted &&
                !is_resizing &&
                !is_maximised &&
                window_resize_bound_containe(this, event->mouse.position))
            {
                window_begin_resize(this, event->mouse.position);
            }
        }

        break;
    }

    case Event::MOUSE_BUTTON_RELEASE:
    {
        Widget *widget = window_child_at(this, event->mouse.position);

        if (widget)
        {
            mouse_focused_widget = widget;
            widget->dispatch_event(event);
        }

        if (is_dragging &&
            event->mouse.button == MOUSE_BUTTON_LEFT)
        {
            is_dragging = false;
            cursor(CURSOR_DEFAULT);
        }

        if (is_resizing)
        {
            window_end_resize(this);
        }

        break;
    }

    case Event::MOUSE_DOUBLE_CLICK:
    {
        if (root()->bound().contains(event->mouse.position))
        {
            Widget *widget = window_child_at(this, event->mouse.position);

            if (widget)
            {
                widget->dispatch_event(event);
            }
        }

        break;
    }

    case Event::KEYBOARD_KEY_TYPED:
    case Event::KEYBOARD_KEY_PRESS:
    case Event::KEYBOARD_KEY_RELEASE:
    {
        if (focused_widget)
        {
            focused_widget->dispatch_event(event);
        }
        break;
    }
    case Event::DISPLAY_SIZE_CHANGED:
        break;
    case Event::WINDOW_MAXIMIZING:
        if (is_maximised == true)
        {
            is_maximised = false;
            bound(previous_bound);
        }
        else
        {
            is_maximised = true;
            previous_bound = _bound;
            Rectangle new_size = Screen::bound();
            new_size = Rectangle(0, WINDOW_HEADER_AREA, new_size.width(), new_size.height() - WINDOW_HEADER_AREA);

            bound(new_size);
        }

        break;
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
    if (is_resizing)
        return;

    if (cursor_state != state)
    {
        application_window_change_cursor(this, state);
        cursor_state = state;
    }
}

void Window::focus_widget(Widget *widget)
{
    focused_widget = widget;
}

void Window::widget_removed(Widget *widget)
{
    if (focused_widget == widget)
    {
        focused_widget = nullptr;
    }
    if (mouse_focused_widget == widget)
    {
        mouse_focused_widget = nullptr;
    }

    if (mouse_over_widget == widget)
    {
        mouse_over_widget = nullptr;
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
    window_populate_header(this);
}

void Window::icon(RefPtr<Icon> icon)
{
    if (icon)
    {
        _icon = icon;
        window_populate_header(this);
    }
}

void Window::bound(Rectangle new_bound)
{
    _bound = new_bound;

    if (!_visible)
        return;

    application_resize_window(this, _bound);
    window_change_framebuffer_if_needed(this);

    should_relayout();
    should_repaint(bound());
}

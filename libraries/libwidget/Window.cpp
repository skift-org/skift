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
            window_event(window, &maximise_event);
        });
    }

    Widget *close_button = new Button(window->header(), BUTTON_TEXT, Icon::get("window-close"));
    close_button->insets(Insets(3));

    close_button->on(Event::ACTION, [window](auto) {
        Event close_event = {};
        close_event.type = Event::WINDOW_CLOSING;
        window_event(window, &close_event);
    });
}

Window::Window(WindowFlag flags)
{
    static int window_handle_counter = 0;
    handle = window_handle_counter++;
    _title = strdup("Window");
    _icon = Icon::get("application");
    _type = WINDOW_TYPE_REGULAR;
    _flags = flags;
    is_maximised = false;
    focused = false;
    cursor_state = CURSOR_DEFAULT;

    frontbuffer = Bitmap::create_shared(250, 250).take_value();
    frontbuffer_painter = own<Painter>(frontbuffer);

    backbuffer = Bitmap::create_shared(250, 250).take_value();
    backbuffer_painter = own<Painter>(backbuffer);

    _bound = Rectangle(250, 250);
    dirty_rect = list_create();

    header_container = new Container(nullptr);
    header_container->window(this);

    window_populate_header(this);

    root_container = new Container(nullptr);
    root_container->window(this);

    focused_widget = root_container;
    widget_by_id = hashmap_create_string_to_value();

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
    if (visible)
    {
        hide();
    }

    application_remove_window(this);

    delete root_container;
    delete header_container;

    list_destroy_with_callback(dirty_rect, free);

    free(_title);
}

void Window::repaint(Rectangle rectangle)
{
    Painter &painter = *backbuffer_painter;

    if (_flags & WINDOW_TRANSPARENT)
    {
        painter.clear_rectangle(rectangle, window_get_color(this, THEME_BACKGROUND).with_alpha(_opacity));
    }
    else
    {
        painter.clear_rectangle(rectangle, window_get_color(this, THEME_BACKGROUND));
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

            painter.draw_rectangle(bound(), window_get_color(this, THEME_ACCENT));
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

    list_foreach(Rectangle, rectangle, dirty_rect)
    {
        repaint(*rectangle);

        if (repaited_regions.is_empty())
        {
            repaited_regions = *rectangle;
        }
        else
        {
            repaited_regions = rectangle->merged_with(repaited_regions);
        }
    }

    list_clear_with_callback(dirty_rect, free);

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
    if (!visible)
        return;

    if (dirty_rect->empty())
    {
        _repaint_invoker->invoke_later();
    }

    list_foreach(Rectangle, dirty_rect, dirty_rect)
    {
        if (dirty_rect->colide_with(rectangle))
        {
            *dirty_rect = dirty_rect->merged_with(rectangle);
            return;
        }
    }

    Rectangle *dirty = __create(Rectangle);
    *dirty = rectangle;
    list_pushback(dirty_rect, dirty);
}

void Window::should_relayout()
{
    if (dirty_layout || !visible)
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

bool window_is_visible(Window *window)
{
    return window->visible;
}

void Window::show()
{
    if (visible)
        return;

    visible = true;

    window_change_framebuffer_if_needed(this);

    relayout();
    repaint(bound());

    application_show_window(this);
}

void Window::hide()
{
    event_cancel_run_later_for(this);

    if (!visible)
        return;

    visible = false;
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

void window_event(Window *window, Event *event)
{
    if (is_mouse_event(event))
    {
        event->mouse.position = event->mouse.position - window->_bound.position();
        event->mouse.old_position = event->mouse.old_position - window->_bound.position();
    }

    if (window->handlers[event->type])
    {
        event->accepted = true;
        window->handlers[event->type](event);
    }

    if (event->accepted)
    {
        return;
    }

    switch (event->type)
    {
    case Event::GOT_FOCUS:
    {
        window->focused = true;

        window->should_repaint(window->bound());
    }
    break;

    case Event::LOST_FOCUS:
    {
        window->focused = false;
        window->should_repaint(window->bound());

        Event mouse_leave = *event;
        mouse_leave.type = Event::MOUSE_LEAVE;

        if (window->mouse_over_widget)
        {
            window->mouse_over_widget->dispatch_event(&mouse_leave);
        }

        if (window->_type == WINDOW_TYPE_POPOVER)
        {
            window->hide();
        }
    }
    break;

    case Event::WINDOW_CLOSING:
    {
        window->hide();
    }
    break;

    case Event::MOUSE_MOVE:
    {
        RectangleBorder borders = window_resize_bound_containe(window, event->mouse.position);

        if (window->is_dragging && !window->is_maximised)
        {
            Vec2i offset = event->mouse.position - event->mouse.old_position;
            window->_bound = window->_bound.offset(offset);
            application_move_window(window, window->_bound.position());
        }
        else if (window->is_resizing && !window->is_maximised)
        {
            window_do_resize(window, event->mouse.position);
        }
        else if (borders && (window->_flags & WINDOW_RESIZABLE) && !window->is_maximised)
        {
            if ((borders & RectangleBorder::TOP) && (borders & RectangleBorder::LEFT))
            {
                window_set_cursor(window, CURSOR_RESIZEHV);
            }
            else if ((borders & RectangleBorder::BOTTOM) && (borders & RectangleBorder::RIGHT))
            {
                window_set_cursor(window, CURSOR_RESIZEHV);
            }
            else if ((borders & RectangleBorder::TOP) && (borders & RectangleBorder::RIGHT))
            {
                window_set_cursor(window, CURSOR_RESIZEVH);
            }
            else if ((borders & RectangleBorder::BOTTOM) && (borders & RectangleBorder::LEFT))
            {
                window_set_cursor(window, CURSOR_RESIZEVH);
            }
            else if (borders & (RectangleBorder::TOP | RectangleBorder::BOTTOM))
            {
                window_set_cursor(window, CURSOR_RESIZEV);
            }
            else if (borders & (RectangleBorder::LEFT | RectangleBorder::RIGHT))
            {
                window_set_cursor(window, CURSOR_RESIZEH);
            }
        }
        else
        {
            // FIXME: Set the cursor based on the focused widget.
            window_set_cursor(window, CURSOR_DEFAULT);

            Widget *mouse_over_widget = window_child_at(window, event->mouse.position);

            if (mouse_over_widget != window->mouse_over_widget)
            {
                Event mouse_leave = *event;
                mouse_leave.type = Event::MOUSE_LEAVE;

                if (window->mouse_over_widget)
                {
                    window->mouse_over_widget->dispatch_event(&mouse_leave);
                }

                Event mouse_enter = *event;
                mouse_enter.type = Event::MOUSE_ENTER;

                if (mouse_over_widget)
                {
                    mouse_over_widget->dispatch_event(&mouse_enter);
                }

                window->mouse_over_widget = mouse_over_widget;
            }

            if (window->mouse_focused_widget)
            {
                window->mouse_focused_widget->dispatch_event(event);
            }
        }

        break;
    }

    case Event::MOUSE_BUTTON_PRESS:
    {
        if (window->root()->bound().contains(event->mouse.position))
        {
            Widget *widget = window_child_at(window, event->mouse.position);

            if (widget)
            {
                window->mouse_focused_widget = widget;
                widget->dispatch_event(event);
            }
        }

        if (!event->accepted && !(window->_flags & WINDOW_BORDERLESS))
        {
            if (!event->accepted && window->header()->bound().contains(event->mouse.position))
            {
                Widget *widget = window->header()->child_at(event->mouse.position);

                if (widget)
                {
                    widget->dispatch_event(event);
                }
            }

            if (!event->accepted &&
                !window->is_dragging &&
                !window->is_maximised &&
                event->mouse.button == MOUSE_BUTTON_LEFT &&
                window_header_bound(window).contains(event->mouse.position))
            {
                window->is_dragging = true;
                window_set_cursor(window, CURSOR_MOVE);
            }

            if ((window->_flags & WINDOW_RESIZABLE) &&
                !event->accepted &&
                !window->is_resizing &&
                !window->is_maximised &&
                window_resize_bound_containe(window, event->mouse.position))
            {
                window_begin_resize(window, event->mouse.position);
            }
        }

        break;
    }

    case Event::MOUSE_BUTTON_RELEASE:
    {
        Widget *widget = window_child_at(window, event->mouse.position);

        if (widget)
        {
            window->mouse_focused_widget = widget;
            widget->dispatch_event(event);
        }

        if (window->is_dragging &&
            event->mouse.button == MOUSE_BUTTON_LEFT)
        {
            window->is_dragging = false;
            window_set_cursor(window, CURSOR_DEFAULT);
        }

        if (window->is_resizing)
        {
            window_end_resize(window);
        }

        break;
    }

    case Event::MOUSE_DOUBLE_CLICK:
    {
        if (window->root()->bound().contains(event->mouse.position))
        {
            Widget *widget = window_child_at(window, event->mouse.position);

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
        if (window->focused_widget)
        {
            window->focused_widget->dispatch_event(event);
        }
        break;
    }
    case Event::DISPLAY_SIZE_CHANGED:
        break;
    case Event::WINDOW_MAXIMIZING:
        if (window->is_maximised == true)
        {
            window->is_maximised = false;
            window->bound(window->previous_bound);
        }
        else
        {
            window->is_maximised = true;
            window->previous_bound = window->_bound;
            Rectangle new_size = Screen::bound();
            new_size = Rectangle(0, WINDOW_HEADER_AREA, new_size.width(), new_size.height() - WINDOW_HEADER_AREA);

            window->bound(new_size);
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

void window_set_cursor(Window *window, CursorState state)
{
    if (window->is_resizing)
        return;

    if (window->cursor_state != state)
    {
        application_window_change_cursor(window, state);
        window->cursor_state = state;
    }
}

void window_set_focused_widget(Window *window, Widget *widget)
{
    window->focused_widget = widget;
}

void window_widget_removed(Window *window, Widget *widget)
{
    if (window->focused_widget == widget)
    {
        window->focused_widget = nullptr;
    }
    if (window->mouse_focused_widget == widget)
    {
        window->mouse_focused_widget = nullptr;
    }

    if (window->mouse_over_widget == widget)
    {
        window->mouse_over_widget = nullptr;
    }

    hashmap_remove_value(window->widget_by_id, widget);
}

void window_register_widget_by_id(Window *window, const char *id, Widget *widget)
{
    if (hashmap_has(window->widget_by_id, id))
    {
        hashmap_remove(window->widget_by_id, id);
    }

    hashmap_put(window->widget_by_id, id, widget);
}

Widget *window_get_widget_by_id(Window *window, const char *id)
{
    if (hashmap_has(window->widget_by_id, id))
    {
        return (Widget *)hashmap_get(window->widget_by_id, id);
    }
    else
    {
        return nullptr;
    }
}

int window_handle(Window *window)
{
    return window->handle;
}

int window_frontbuffer_handle(Window *window)
{
    return window->frontbuffer->handle();
}

int window_backbuffer_handle(Window *window)
{
    return window->backbuffer->handle();
}

Widget *window_header(Window *window)
{
    return window->header_container;
}

Widget *window_root(Window *window)
{
    return window->root_container;
}

bool window_is_focused(Window *window)
{
    if (window->_flags & WINDOW_ALWAYS_FOCUSED)
    {
        return true;
    }
    else
    {
        return window->focused;
    }
}

Color window_get_color(Window *window, ThemeColorRole role)
{
    if (!window_is_focused(window))
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

void Window::title(const char *title)
{
    if (_title)
    {
        free(_title);
        _title = strdup(title);

        window_populate_header(this);
    }
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

    if (!visible)
        return;

    application_resize_window(this, _bound);

    window_change_framebuffer_if_needed(this);

    should_relayout();
    should_repaint(bound());
}

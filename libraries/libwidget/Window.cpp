#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/io/Stream.h>
#include <libsystem/system/Memory.h>
#include <libwidget/Application.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>
#include <libwidget/Widgets.h>
#include <libwidget/Window.h>

#define WINDOW_RESIZE_AREA 16
#define WINDOW_HEADER_AREA 36
#define WINDOW_CONTENT_PADDING 1

static void close_button_click(void *target, struct Widget *sender, struct Event *event)
{
    __unused(target);
    __unused(event);

    Event close_event = {};

    close_event.type = EVENT_WINDOW_CLOSING;

    window_event(sender->window, &close_event);
}

void window_populate_header(Window *window)
{
    widget_clear_childs(window->header_container);

    window_header(window)->layout = HFLOW(4);
    window_header(window)->insets = Insets(6, 6);

    button_create_with_icon_and_text(
        window_header(window),
        BUTTON_TEXT,
        window->icon,
        window->title);

    container_create(window_header(window))
        ->layout_attributes = LAYOUT_FILL;

    if (window->flags & WINDOW_RESIZABLE)
    {
        Widget *button_minimize = button_create_with_icon(window_header(window), BUTTON_TEXT, icon_get("window-minimize"));
        button_minimize->insets = Insets(3);

        Widget *button_maximize = button_create_with_icon(window_header(window), BUTTON_TEXT, icon_get("window-maximize"));
        button_maximize->insets = Insets(3);
    }

    Widget *close_button = button_create_with_icon(window_header(window), BUTTON_TEXT, icon_get("window-close"));
    close_button->insets = Insets(3);

    widget_set_event_handler(close_button, EVENT_ACTION, EVENT_HANDLER(NULL, close_button_click));
}

void window_initialize(Window *window, WindowFlag flags)
{
    static int window_handle_counter = 0;
    window->handle = window_handle_counter++;
    window->title = strdup("Window");
    window->icon = icon_get("application");
    window->flags = flags;

    window->focused = false;
    window->cursor_state = CURSOR_DEFAULT;

    window->frontbuffer = bitmap_create(250, 250);
    window->frontbuffer_painter = painter_create(window->frontbuffer);
    shared_memory_get_handle((uintptr_t)window->frontbuffer, &window->frontbuffer_handle);

    window->backbuffer = bitmap_create(250, 250);
    window->backbuffer_painter = painter_create(window->backbuffer);
    shared_memory_get_handle((uintptr_t)window->backbuffer, &window->backbuffer_handle);

    window->on_screen_bound = Rectangle(250, 250);
    window->dirty_rect = list_create();

    window->header_container = container_create(NULL);
    window->header_container->window = window;

    window_populate_header(window);

    window->root_container = container_create(NULL);
    window->root_container->window = window;

    window->focused_widget = window->root_container;
    window->widget_by_id = hashmap_create_string_to_value();

    application_add_window(window);
    window_set_position(window, Vec2i(96, 72));
}

Window *window_create(WindowFlag flags)
{
    Window *window = __create(Window);

    window_initialize(window, flags);

    return window;
}

void window_destroy(Window *window)
{
    if (window->visible)
    {
        window_hide(window);
    }

    application_remove_window(window);

    widget_destroy(window->root_container);
    widget_destroy(window->header_container);

    painter_destroy(window->frontbuffer_painter);
    bitmap_destroy(window->frontbuffer);

    painter_destroy(window->backbuffer_painter);
    bitmap_destroy(window->backbuffer);

    list_destroy_with_callback(window->dirty_rect, free);

    if (window->destroy)
    {
        window->destroy(window);
    }

    free(window->title);
    free(window);
}

void window_set_title(Window *window, const char *title)
{
    if (title)
    {
        free(window->title);
        window->title = strdup(title);

        window_populate_header(window);
    }
}

void window_set_icon(Window *window, Icon *icon)
{
    if (icon)
    {
        window->icon = icon;
        window_populate_header(window);
    }
}

void window_set_size(Window *window, Vec2i size)
{
    window_set_bound(window, window->on_screen_bound.resized(size));
}

void window_set_position(Window *window, Vec2i position)
{
    window_set_bound(window, window->on_screen_bound.moved(position));
}

static void window_change_framebuffer_if_needed(Window *window)
{
    if (window_bound(window).width() != bitmap_bound(window->frontbuffer).width() ||
        window_bound(window).height() != bitmap_bound(window->frontbuffer).height())
    {
        painter_destroy(window->frontbuffer_painter);
        bitmap_destroy(window->frontbuffer);

        painter_destroy(window->backbuffer_painter);
        bitmap_destroy(window->backbuffer);

        window->frontbuffer = bitmap_create(window_bound(window).width(), window_bound(window).height());
        window->frontbuffer_painter = painter_create(window->frontbuffer);
        shared_memory_get_handle((uintptr_t)window->frontbuffer, &window->frontbuffer_handle);

        window->backbuffer = bitmap_create(window_bound(window).width(), window_bound(window).height());
        window->backbuffer_painter = painter_create(window->backbuffer);
        shared_memory_get_handle((uintptr_t)window->backbuffer, &window->backbuffer_handle);
    }
}

void window_set_bound(Window *window, Rectangle bound)
{
    window->on_screen_bound = bound;

    if (!window->visible)
        return;

    application_resize_window(window, window->on_screen_bound);

    window_change_framebuffer_if_needed(window);
    window_schedule_layout(window);
    window_schedule_update(window, window_bound(window));
}

bool window_is_visible(Window *window)
{
    return window->visible;
}

void window_show(Window *window)
{
    if (window->visible)
        return;

    window->visible = true;

    window_change_framebuffer_if_needed(window);
    window_schedule_layout(window);
    window_schedule_update(window, window_bound(window));
    application_show_window(window);
}

void window_show_dialog(Window *window)
{
    window_show(window);
}

void window_hide(Window *window)
{
    event_cancel_run_later_for(window);

    if (!window->visible)
        return;

    window->visible = false;
    application_hide_window(window);
}

Rectangle window_header_bound(Window *window)
{
    return window_bound(window).take_top(WINDOW_HEADER_AREA);
}

Rectangle window_header_bound_on_screen(Window *window)
{
    return window_bound_on_screen(window).take_top(WINDOW_HEADER_AREA);
}

Rectangle window_content_bound(Window *window)
{
    if (window->flags & WINDOW_BORDERLESS)
    {
        return window_bound(window);
    }
    else
    {
        return window_bound(window).shrinked(Insets(WINDOW_HEADER_AREA, WINDOW_CONTENT_PADDING, WINDOW_CONTENT_PADDING));
    }
}

void window_paint(Window *window, Painter *painter, Rectangle rectangle)
{
    painter_clear_rectangle(painter, rectangle, window_get_color(window, THEME_BACKGROUND));

    painter_push_clip(painter, rectangle);

    if (window_content_bound(window).containe(rectangle))
    {
        if (window_root(window))
        {
            widget_paint(window_root(window), painter, rectangle);
        }
    }
    else
    {
        if (window_root(window))
        {
            widget_paint(window_root(window), painter, rectangle);
        }

        if (!(window->flags & WINDOW_BORDERLESS))
        {
            if (window_header(window))
            {
                widget_paint(window_header(window), painter, rectangle);
            }

            painter_draw_rectangle(painter, window_bound(window), window_get_color(window, THEME_ACCENT));
        }
    }

    painter_pop_clip(painter);
}

void window_dump(Window *window)
{
    printf("Window(0x%p) (%d, %d) %dx%d\n",
           window,
           window->on_screen_bound.x(),
           window->on_screen_bound.y(),
           window->on_screen_bound.width(),
           window->on_screen_bound.height());

    widget_dump(window->root_container, 1);
}

RectangleBorder window_resize_bound_containe(Window *window, Vec2i position)
{
    Rectangle resize_bound = window_bound(window).expended(Insets(WINDOW_RESIZE_AREA));
    return resize_bound.containe(Insets(WINDOW_RESIZE_AREA), position);
}

void window_begin_resize(Window *window, Vec2i mouse_position)
{
    window->is_resizing = true;

    RectangleBorder borders = window_resize_bound_containe(window, mouse_position);

    window->resize_horizontal = borders & (RectangleBorder::LEFT | RectangleBorder::RIGHT);
    window->resize_vertical = borders & (RectangleBorder::TOP | RectangleBorder::BOTTOM);

    Vec2i resize_region_begin(
        window_bound_on_screen(window).x(),
        window_bound_on_screen(window).y());

    if (borders & RectangleBorder::TOP)
    {
        resize_region_begin += Vec2i(0, window_bound_on_screen(window).height());
    }

    if (borders & RectangleBorder::LEFT)
    {
        resize_region_begin += Vec2i(window_bound_on_screen(window).width(), 0);
    }

    window->resize_begin = resize_region_begin;
}

void window_do_resize(Window *window, Vec2i mouse_position)
{
    Rectangle new_bound = Rectangle::from_two_point(
        window->resize_begin,
        window_bound_on_screen(window).position() + mouse_position);

    if (!window->resize_horizontal)
    {
        new_bound = new_bound
                        .moved({window_bound_on_screen(window).x(), new_bound.y()})
                        .with_width(window_bound_on_screen(window).width());
    }

    if (!window->resize_vertical)
    {
        new_bound = new_bound
                        .moved({new_bound.x(), window_bound_on_screen(window).y()})
                        .with_height(window_bound_on_screen(window).height());
    }

    Vec2i content_size = widget_compute_size(window_root(window));

    new_bound = new_bound.with_width(MAX(new_bound.width(), MAX(widget_compute_size(window_header(window)).x(), content_size.x()) + WINDOW_CONTENT_PADDING * 2));
    new_bound = new_bound.with_height(MAX(new_bound.height(), WINDOW_HEADER_AREA + content_size.y() + WINDOW_CONTENT_PADDING));

    window_set_bound(window, new_bound);
}

void window_end_resize(Window *window)
{
    window->is_resizing = false;
}

Widget *window_child_at(Window *window, Vec2i position)
{
    if (widget_get_bound(window_root(window)).containe(position))
    {
        return widget_get_child_at(window_root(window), position);
    }

    if (widget_get_bound(window_header(window)).containe(position))
    {
        return widget_get_child_at(window_header(window), position);
    }

    return NULL;
}

void window_event(Window *window, Event *event)
{
    if (is_mouse_event(event))
    {
        event->mouse.position = event->mouse.position - window->on_screen_bound.position();
        event->mouse.old_position = event->mouse.old_position - window->on_screen_bound.position();
    }

    if (window->handlers[event->type].callback != NULL)
    {
        event->accepted = true;
        window->handlers[event->type].callback(
            window->handlers[event->type].target,
            window,
            event);
    }

    if (event->accepted)
    {
        return;
    }

    switch (event->type)
    {
    case EVENT_GOT_FOCUS:
    {
        window->focused = true;
        window_schedule_update(window, window_bound(window));
    }
    break;

    case EVENT_LOST_FOCUS:
    {
        window->focused = false;
        window_schedule_update(window, window_bound(window));

        Event mouse_leave = *event;
        mouse_leave.type = EVENT_MOUSE_LEAVE;

        if (window->mouse_over_widget)
        {
            widget_event(window->mouse_over_widget, &mouse_leave);
        }

        if (window->flags & WINDOW_POP_OVER)
        {
            window_hide(window);
        }
    }
    break;

    case EVENT_WINDOW_CLOSING:
    {
        window_hide(window);
    }
    break;

    case EVENT_MOUSE_MOVE:
    {
        RectangleBorder borders = window_resize_bound_containe(window, event->mouse.position);

        if (window->is_dragging)
        {
            Vec2i offset = event->mouse.position - event->mouse.old_position;
            window->on_screen_bound = window->on_screen_bound.offset(offset);
            application_move_window(window, window->on_screen_bound.position());
        }
        else if (window->is_resizing)
        {
            window_do_resize(window, event->mouse.position);
        }
        else if (borders && (window->flags & WINDOW_RESIZABLE))
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
                mouse_leave.type = EVENT_MOUSE_LEAVE;

                if (window->mouse_over_widget)
                {
                    widget_event(window->mouse_over_widget, &mouse_leave);
                }

                Event mouse_enter = *event;
                mouse_enter.type = EVENT_MOUSE_ENTER;

                if (mouse_over_widget)
                {
                    widget_event(mouse_over_widget, &mouse_enter);
                }

                window->mouse_over_widget = mouse_over_widget;
            }

            if (window->mouse_focused_widget)
            {
                widget_event(window->mouse_focused_widget, event);
            }
        }

        break;
    }

    case EVENT_MOUSE_BUTTON_PRESS:
    {
        if (widget_get_bound(window_root(window)).containe(event->mouse.position))
        {
            Widget *widget = window_child_at(window, event->mouse.position);

            if (widget)
            {
                window->mouse_focused_widget = widget;
                widget_event(widget, event);
            }
        }

        if (!event->accepted && !(window->flags & WINDOW_BORDERLESS))
        {
            if (!event->accepted && widget_get_bound(window_header(window)).containe(event->mouse.position))
            {
                Widget *widget = widget_get_child_at(window_header(window), event->mouse.position);

                if (widget)
                {
                    widget_event(widget, event);
                }
            }

            if (!event->accepted &&
                !window->is_dragging &&
                event->mouse.button == MOUSE_BUTTON_LEFT &&
                window_header_bound(window).containe(event->mouse.position))
            {
                window->is_dragging = true;
                window_set_cursor(window, CURSOR_MOVE);
            }

            if ((window->flags & WINDOW_RESIZABLE) &&
                !event->accepted &&
                !window->is_resizing &&
                window_resize_bound_containe(window, event->mouse.position))
            {
                window_begin_resize(window, event->mouse.position);
            }
        }

        break;
    }

    case EVENT_MOUSE_BUTTON_RELEASE:
    {
        Widget *widget = window_child_at(window, event->mouse.position);

        if (widget)
        {
            window->mouse_focused_widget = widget;
            widget_event(widget, event);
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

    case EVENT_MOUSE_DOUBLE_CLICK:
    {
        if (widget_get_bound(window_root(window)).containe(event->mouse.position))
        {
            Widget *widget = window_child_at(window, event->mouse.position);

            if (widget)
            {
                widget_event(widget, event);
            }
        }

        break;
    }

    case EVENT_KEYBOARD_KEY_TYPED:
    case EVENT_KEYBOARD_KEY_PRESS:
    case EVENT_KEYBOARD_KEY_RELEASE:
    {
        if (window->focused_widget)
        {
            widget_event(window->focused_widget, event);
        }
        break;
    }

    default:
        break;
    }
}

void window_set_event_handler(Window *window, EventType event, EventHandler handler)
{
    assert(event < __EVENT_TYPE_COUNT);
    window->handlers[event] = handler;
}

Rectangle window_bound_on_screen(Window *window)
{
    return window->on_screen_bound;
}

void window_set_on_screen_bound(Window *window, Rectangle new_bound)
{
    window->on_screen_bound = new_bound;

    if (!window->visible)
        return;

    application_resize_window(window, new_bound);
}

Rectangle window_bound(Window *window)
{
    return window->on_screen_bound.moved({0, 0});
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
        window->focused_widget = NULL;
    }

    if (window->mouse_focused_widget == widget)
    {
        window->mouse_focused_widget = NULL;
    }

    if (window->mouse_over_widget == widget)
    {
        window->mouse_over_widget = NULL;
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
        return NULL;
    }
}

int window_handle(Window *window)
{
    return window->handle;
}

int window_frontbuffer_handle(Window *window)
{
    return window->frontbuffer_handle;
}

int window_backbuffer_handle(Window *window)
{
    return window->backbuffer_handle;
}

Widget *window_header(Window *window)
{
    return window->header_container;
}

Widget *window_root(Window *window)
{
    return window->root_container;
}

void window_update(Window *window)
{
    Rectangle repaited_regions = Rectangle::empty();

    list_foreach(Rectangle, rectangle, window->dirty_rect)
    {
        window_paint(window, window->backbuffer_painter, *rectangle);

        if (repaited_regions.is_empty())
        {
            repaited_regions = *rectangle;
        }
        else
        {
            repaited_regions = rectangle->merged_with(repaited_regions);
        }
    }

    list_clear_with_callback(window->dirty_rect, free);

    bitmap_copy(window->backbuffer, window->frontbuffer, repaited_regions);

    __swap(Bitmap *, window->frontbuffer, window->backbuffer);
    __swap(Painter *, window->frontbuffer_painter, window->backbuffer_painter);
    __swap(int, window->frontbuffer_handle, window->backbuffer_handle);

    application_flip_window(window, repaited_regions);
}

void window_schedule_update(Window *window, Rectangle rectangle)
{
    if (!window->visible)
        return;

    if (list_count(window->dirty_rect) == 0)
    {
        eventloop_run_later((RunLaterCallback)window_update, window);
    }

    list_foreach(Rectangle, dirty_rect, window->dirty_rect)
    {
        if (dirty_rect->colide_with(rectangle))
        {
            *dirty_rect = dirty_rect->merged_with(rectangle);
            return;
        }
    }

    Rectangle *dirty_rect = __create(Rectangle);

    *dirty_rect = rectangle;

    list_pushback(window->dirty_rect, dirty_rect);
}

void window_layout(Window *window)
{
    window_header(window)->bound = window_header_bound(window);
    widget_layout(window_header(window));

    window_root(window)->bound = window_content_bound(window);
    widget_layout(window_root(window));

    window->dirty_layout = false;
}

void window_schedule_layout(Window *window)
{
    if (window->dirty_layout || !window->visible)
        return;

    window->dirty_layout = true;

    eventloop_run_later((RunLaterCallback)window_layout, window);
}

bool window_is_focused(Window *window)
{
    if (window->flags & WINDOW_ALWAYS_FOCUSED)
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

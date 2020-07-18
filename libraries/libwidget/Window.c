#include <libsystem/Assert.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/Memory.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/io/Stream.h>
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

    Event close_event = {
        .type = EVENT_WINDOW_CLOSING,
    };

    window_event(sender->window, &close_event);
}

void window_populate_header(Window *window)
{
    widget_clear_childs(window->header_container);

    window_header(window)->layout = HFLOW(4);
    window_header(window)->insets = INSETS(6, 6);

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
        button_minimize->insets = INSETS(3);

        Widget *button_maximize = button_create_with_icon(window_header(window), BUTTON_TEXT, icon_get("window-maximize"));
        button_maximize->insets = INSETS(3);
    }

    Widget *close_button = button_create_with_icon(window_header(window), BUTTON_TEXT, icon_get("window-close"));
    close_button->insets = INSETS(3);

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

    window->on_screen_bound = RECTANGLE_SIZE(250, 250);
    window->dirty_rect = list_create();

    window->header_container = container_create(NULL);
    window->header_container->window = window;

    window_populate_header(window);

    window->root_container = container_create(NULL);
    window->root_container->window = window;

    window->focused_widget = window->root_container;
    window->widget_by_id = hashmap_create_string_to_value();

    application_add_window(window);
    window_set_position(window, vec2i(96, 72));
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
    Rectangle bound = window->on_screen_bound;

    bound.size = size;

    window_set_bound(window, bound);
}

void window_set_position(Window *window, Vec2i position)
{
    Rectangle bound = window->on_screen_bound;

    bound.position = position;

    window_set_bound(window, bound);
}

static void window_change_framebuffer_if_needed(Window *window)
{
    if (window_bound(window).width != bitmap_bound(window->frontbuffer).width ||
        window_bound(window).height != bitmap_bound(window->frontbuffer).height)
    {
        painter_destroy(window->frontbuffer_painter);
        bitmap_destroy(window->frontbuffer);

        painter_destroy(window->backbuffer_painter);
        bitmap_destroy(window->backbuffer);

        window->frontbuffer = bitmap_create(window_bound(window).width, window_bound(window).height);
        window->frontbuffer_painter = painter_create(window->frontbuffer);
        shared_memory_get_handle((uintptr_t)window->frontbuffer, &window->frontbuffer_handle);

        window->backbuffer = bitmap_create(window_bound(window).width, window_bound(window).height);
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
    return rectangle_set_height(window_bound(window), WINDOW_HEADER_AREA);
}

Rectangle window_header_bound_on_screen(Window *window)
{
    return rectangle_set_height(window_bound_on_screen(window), WINDOW_HEADER_AREA);
}

Rectangle window_content_bound(Window *window)
{
    if (window->flags & WINDOW_BORDERLESS)
    {
        return window_bound(window);
    }
    else
    {
        return rectangle_shrink(window_bound(window), INSETS(WINDOW_HEADER_AREA, WINDOW_CONTENT_PADDING, WINDOW_CONTENT_PADDING));
    }
}

void window_paint(Window *window, Painter *painter, Rectangle rectangle)
{
    painter_clear_rectangle(painter, rectangle, ALPHA(window_get_color(window, THEME_BACKGROUND), 0.8));

    painter_push_clip(painter, rectangle);

    if (rectangle_container_rectangle(window_content_bound(window), rectangle))
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
           window->on_screen_bound.x,
           window->on_screen_bound.y,
           window->on_screen_bound.width,
           window->on_screen_bound.height);

    widget_dump(window->root_container, 1);
}

RectangeBorder window_resize_bound_containe(Window *window, Vec2i position)
{
    Rectangle resize_bound = rectangle_expand(
        window_bound(window),
        INSETS(WINDOW_RESIZE_AREA));

    return rectangle_inset_containe_point(
        resize_bound,
        INSETS(WINDOW_RESIZE_AREA),
        position);
}

void window_begin_resize(Window *window, Vec2i mouse_position)
{
    window->is_resizing = true;

    RectangeBorder borders = window_resize_bound_containe(window, mouse_position);

    window->resize_horizontal = borders & (RECTANGLE_BORDER_LEFT | RECTANGLE_BORDER_RIGHT);
    window->resize_vertical = borders & (RECTANGLE_BORDER_TOP | RECTANGLE_BORDER_BOTTOM);

    Vec2i resize_region_begin = {};

    if (borders & RECTANGLE_BORDER_TOP)
    {
        resize_region_begin.y = window_bound_on_screen(window).y + window_bound_on_screen(window).height;
    }

    if (borders & RECTANGLE_BORDER_BOTTOM)
    {
        resize_region_begin.y = window_bound_on_screen(window).y;
    }

    if (borders & RECTANGLE_BORDER_LEFT)
    {
        resize_region_begin.x = window_bound_on_screen(window).x + window_bound_on_screen(window).width;
    }

    if (borders & RECTANGLE_BORDER_RIGHT)
    {
        resize_region_begin.x = window_bound_on_screen(window).x;
    }

    window->resize_begin = resize_region_begin;
}

void window_do_resize(Window *window, Vec2i mouse_position)
{
    Rectangle new_bound = rectangle_from_two_point(window->resize_begin, vec2i_add(window_bound_on_screen(window).position, mouse_position));

    if (!window->resize_horizontal)
    {
        new_bound.x = window_bound_on_screen(window).x;
        new_bound.width = window_bound_on_screen(window).width;
    }

    if (!window->resize_vertical)
    {
        new_bound.y = window_bound_on_screen(window).y;
        new_bound.height = window_bound_on_screen(window).height;
    }

    Vec2i content_size = widget_compute_size(window_root(window));

    new_bound.height = MAX(new_bound.height, WINDOW_HEADER_AREA + content_size.height + WINDOW_CONTENT_PADDING);
    new_bound.width = MAX(new_bound.width, MAX(widget_compute_size(window_header(window)).width, content_size.width) + WINDOW_CONTENT_PADDING * 2);

    window_set_bound(window, new_bound);
}

void window_end_resize(Window *window)
{
    window->is_resizing = false;
}

Widget *window_child_at(Window *window, Vec2i position)
{
    if (rectangle_containe_point(widget_get_bound(window_root(window)), position))
    {
        return widget_get_child_at(window_root(window), position);
    }

    if (rectangle_containe_point(widget_get_bound(window_header(window)), position))
    {
        return widget_get_child_at(window_header(window), position);
    }

    return NULL;
}

void window_event(Window *window, Event *event)
{
    if (is_mouse_event(event))
    {
        event->mouse.position = vec2i_sub(event->mouse.position, window->on_screen_bound.position);
        event->mouse.old_position = vec2i_sub(event->mouse.old_position, window->on_screen_bound.position);
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
        RectangeBorder borders = window_resize_bound_containe(window, event->mouse.position);

        if (window->is_dragging)
        {
            Vec2i offset = vec2i_sub(event->mouse.position, event->mouse.old_position);
            window->on_screen_bound = rectangle_offset(window->on_screen_bound, offset);
            application_move_window(window, window->on_screen_bound.position);
        }
        else if (window->is_resizing)
        {
            window_do_resize(window, event->mouse.position);
        }
        else if (borders && (window->flags & WINDOW_RESIZABLE))
        {
            if ((borders & RECTANGLE_BORDER_TOP) && (borders & RECTANGLE_BORDER_LEFT))
            {
                window_set_cursor(window, CURSOR_RESIZEHV);
            }
            else if ((borders & RECTANGLE_BORDER_BOTTOM) && (borders & RECTANGLE_BORDER_RIGHT))
            {
                window_set_cursor(window, CURSOR_RESIZEHV);
            }
            else if ((borders & RECTANGLE_BORDER_TOP) && (borders & RECTANGLE_BORDER_RIGHT))
            {
                window_set_cursor(window, CURSOR_RESIZEVH);
            }
            else if ((borders & RECTANGLE_BORDER_BOTTOM) && (borders & RECTANGLE_BORDER_LEFT))
            {
                window_set_cursor(window, CURSOR_RESIZEVH);
            }
            else if (borders & (RECTANGLE_BORDER_TOP | RECTANGLE_BORDER_BOTTOM))
            {
                window_set_cursor(window, CURSOR_RESIZEV);
            }
            else if (borders & (RECTANGLE_BORDER_LEFT | RECTANGLE_BORDER_RIGHT))
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
        if (rectangle_containe_point(widget_get_bound(window_root(window)), event->mouse.position))
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
            if (!event->accepted && rectangle_containe_point(widget_get_bound(window_header(window)), event->mouse.position))
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
                rectangle_containe_point(window_header_bound(window), event->mouse.position))
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
        if (rectangle_containe_point(widget_get_bound(window_root(window)), event->mouse.position))
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
    Rectangle bound = {};

    bound.size = window->on_screen_bound.size;

    return bound;
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
    Rectangle repaited_regions = RECTANGLE_EMPTY;

    list_foreach(Rectangle, rectangle, window->dirty_rect)
    {
        window_paint(window, window->backbuffer_painter, *rectangle);

        if (rectangle_is_empty(repaited_regions))
        {
            repaited_regions = *rectangle;
        }
        else
        {
            repaited_regions = rectangle_merge(*rectangle, repaited_regions);
        }
    }

    list_clear_with_callback(window->dirty_rect, free);

    bitmap_copy(window->backbuffer, window->frontbuffer, repaited_regions);

    __swap(window->frontbuffer, window->backbuffer);
    __swap(window->frontbuffer_painter, window->backbuffer_painter);
    __swap(window->frontbuffer_handle, window->backbuffer_handle);

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
        if (rectangle_colide(*dirty_rect, rectangle))
        {
            *dirty_rect = rectangle_merge(*dirty_rect, rectangle);
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

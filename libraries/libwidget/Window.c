#include <libsystem/cstring.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/memory.h>
#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Event.h>
#include <libwidget/Icon.h>
#include <libwidget/Label.h>
#include <libwidget/Panel.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>

#define WINDOW_RESIZE_AREA 16
#define WINDOW_HEADER_AREA 32
#define WINDOW_CONTENT_PADDING 1

void close_button_click(void *target, struct Widget *sender, struct Event *event)
{
    __unused(target);
    __unused(event);

    window_hide(sender->window);
}

Window *window_create(
    const char *icon,
    const char *title,
    int width,
    int height,
    WindowFlag flags)
{
    Window *window = __create(Window);

    window_initialize(window, icon, title, width, height, flags);

    return window;
}

void window_initialize(
    Window *window,
    const char *icon,
    const char *title,
    int width,
    int height,
    WindowFlag flags)
{
    static int window_handle_counter = 0;
    window->handle = window_handle_counter++;
    window->focused = false;
    window->cursor_state = CURSOR_DEFAULT;

    window->framebuffer = bitmap_create(width, height);
    window->painter = painter_create(window->framebuffer);
    window->on_screen_bound = RECTANGLE_SIZE(width, height);
    window->dirty_rect = list_create();

    window->header_container = container_create(NULL);
    window->header_container->window = window;

    window_header(window)->layout = (Layout){LAYOUT_HFLOW, 4, 0};
    window_header(window)->insets = INSETS(0, 2, 8);

    icon_create(window_header(window), icon ? icon : "window");
    label_create(window_header(window), title);

    container_create(window_header(window))->layout_attributes = LAYOUT_FILL;

    if (flags & WINDOW_RESIZABLE)
    {
        icon_create(window_header(window), "window-minimize");
        icon_create(window_header(window), "window-maximize");
    }

    Widget *close_button = icon_create(window_header(window), "window-close");
    widget_set_event_handler(close_button, EVENT_MOUSE_BUTTON_PRESS, NULL, close_button_click);

    window->root_container = container_create(NULL);
    window->root_container->window = window;

    window->focused_widget = window->root_container;

    shared_memory_get_handle((uintptr_t)window->framebuffer, &window->framebuffer_handle);

    window->background = window_get_color(window, THEME_BACKGROUND);
    window->flags = flags;

    application_add_window(window);
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

    painter_destroy(window->painter);
    bitmap_destroy(window->framebuffer);

    list_destroy_with_callback(window->dirty_rect, free);

    if (window->destroy)
    {
        window->destroy(window);
    }

    free(window);
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

    window_schedule_layout(window);
    window_schedule_update(window, window_bound(window));
    application_show_window(window);
}

void window_hide(Window *window)
{
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

void window_change_framebuffer_if_needed(Window *window)
{
    if (window_bound(window).width != bitmap_bound(window->framebuffer).width ||
        window_bound(window).height != bitmap_bound(window->framebuffer).height)
    {
        logger_info("Changing framebuffer");

        painter_destroy(window->painter);
        bitmap_destroy(window->framebuffer);

        window->framebuffer = bitmap_create(window_bound(window).width, window_bound(window).height);
        window->painter = painter_create(window->framebuffer);
        shared_memory_get_handle((uintptr_t)window->framebuffer, &window->framebuffer_handle);
    }
}

void window_paint(Window *window, Rectangle rectangle)
{
    painter_clear_rectangle(window->painter, rectangle, window->background);

    if (rectangle_container_rectangle(window_content_bound(window), rectangle))
    {
        if (window_root(window))
        {
            widget_paint(window_root(window), window->painter, rectangle);
        }
    }
    else
    {
        if (window_root(window))
        {
            widget_paint(window_root(window), window->painter, rectangle);
        }

        if (!(window->flags & WINDOW_BORDERLESS))
        {
            if (window_header(window))
            {
                widget_paint(window_header(window), window->painter, rectangle);
            }

            painter_draw_rectangle(window->painter, window_bound(window), window_get_color(window, THEME_ACCENT));
        }
    }
}

void window_dump(Window *window)
{
    printf("Window(0x%p) (%d, %d) %dx%d\n",
           window,
           window->on_screen_bound.X,
           window->on_screen_bound.Y,
           window->on_screen_bound.width,
           window->on_screen_bound.height);

    widget_dump(window->root_container, 1);
}

RectangeBorder window_resize_bound_containe(Window *window, Point position)
{
    Rectangle resize_bound = rectangle_expand(
        window_bound(window),
        INSETS(WINDOW_RESIZE_AREA));

    return rectangle_inset_containe_point(
        resize_bound,
        INSETS(WINDOW_RESIZE_AREA),
        position);
}

void window_begin_resize(Window *window, Point mouse_position)
{
    logger_info("Window begin resize");
    window->is_resizing = true;

    RectangeBorder borders = window_resize_bound_containe(window, mouse_position);

    window->resize_horizontal = borders & (RECTANGLE_BORDER_LEFT | RECTANGLE_BORDER_RIGHT);
    window->resize_vertical = borders & (RECTANGLE_BORDER_TOP | RECTANGLE_BORDER_BOTTOM);

    Point resize_region_begin = {};

    if (borders & RECTANGLE_BORDER_TOP)
    {
        resize_region_begin.Y = window_bound_on_screen(window).Y + window_bound_on_screen(window).height;
    }

    if (borders & RECTANGLE_BORDER_BOTTOM)
    {
        resize_region_begin.Y = window_bound_on_screen(window).Y;
    }

    if (borders & RECTANGLE_BORDER_LEFT)
    {
        resize_region_begin.X = window_bound_on_screen(window).X + window_bound_on_screen(window).width;
    }

    if (borders & RECTANGLE_BORDER_RIGHT)
    {
        resize_region_begin.X = window_bound_on_screen(window).X;
    }

    window->resize_begin = resize_region_begin;
}

void window_do_resize(Window *window, Point mouse_position)
{

    Rectangle new_bound = rectangle_from_two_point(window->resize_begin, point_add(window_bound_on_screen(window).position, mouse_position));

    if (!window->resize_horizontal)
    {
        new_bound.X = window_bound_on_screen(window).X;
        new_bound.width = window_bound_on_screen(window).width;
    }

    if (!window->resize_vertical)
    {
        new_bound.Y = window_bound_on_screen(window).Y;
        new_bound.height = window_bound_on_screen(window).height;
    }

    window_set_on_screen_bound(window, new_bound);
}

void window_end_resize(Window *window)
{
    logger_info("Window end resize");
    window->is_resizing = false;

    window_change_framebuffer_if_needed(window);
    window_schedule_layout(window);
    window_schedule_update(window, window_bound(window));
}

void window_handle_event(Window *window, Event *event)
{
    switch (event->type)
    {
    case EVENT_PAINT:
        window_paint(window, window_bound(window));
        application_blit_window(window, window_bound(window));
        break;

    case EVENT_GOT_FOCUS:
    {
        window->focused = true;
        Event paint_event = {EVENT_PAINT, false};
        window_handle_event(window, &paint_event);
    }
    break;

    case EVENT_LOST_FOCUS:
    {
        window->focused = false;
        Event paint_event = {EVENT_PAINT, false};
        window_handle_event(window, &paint_event);
    }
    break;

    case EVENT_MOUSE_MOVE:
    {
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (window->is_dragging)
        {
            Point offset = point_sub(mouse_event->position, mouse_event->old_position);
            window->on_screen_bound = rectangle_offset(window->on_screen_bound, offset);
            application_move_window(window, window->on_screen_bound.position);
        }
        else if (window->flags & WINDOW_RESIZABLE)
        {
            RectangeBorder borders = window_resize_bound_containe(window, mouse_event->position);

            if (borders)
            {
                if (borders & (RECTANGLE_BORDER_TOP | RECTANGLE_BORDER_BOTTOM))
                {
                    window_set_cursor(window, CURSOR_RESIZEV);
                }

                if (borders & (RECTANGLE_BORDER_LEFT | RECTANGLE_BORDER_RIGHT))
                {
                    window_set_cursor(window, CURSOR_RESIZEH);
                }

                if ((borders & RECTANGLE_BORDER_TOP) && (borders & RECTANGLE_BORDER_LEFT))
                {
                    window_set_cursor(window, CURSOR_RESIZEHV);
                }

                if ((borders & RECTANGLE_BORDER_BOTTOM) && (borders & RECTANGLE_BORDER_RIGHT))
                {
                    window_set_cursor(window, CURSOR_RESIZEHV);
                }

                if ((borders & RECTANGLE_BORDER_TOP) && (borders & RECTANGLE_BORDER_RIGHT))
                {
                    window_set_cursor(window, CURSOR_RESIZEVH);
                }

                if ((borders & RECTANGLE_BORDER_BOTTOM) && (borders & RECTANGLE_BORDER_LEFT))
                {
                    window_set_cursor(window, CURSOR_RESIZEVH);
                }
            }
            else
            {
                window_set_cursor(window, CURSOR_DEFAULT);
            }

            if (window->is_resizing)
            {
                window_do_resize(window, mouse_event->position);
            }
        }

        break;
    }

    case EVENT_MOUSE_BUTTON_PRESS:
    {
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (rectangle_containe_point(widget_bound(window_root(window)), mouse_event->position))
        {
            Widget *widget = widget_child_at(window_root(window), mouse_event->position);

            if (widget)
            {
                widget_dispatch_event(widget, event);
            }
        }

        if (!(window->flags & WINDOW_BORDERLESS))
        {
            if (!event->accepted && rectangle_containe_point(widget_bound(window_header(window)), mouse_event->position))
            {
                Widget *widget = widget_child_at(window_header(window), mouse_event->position);

                if (widget)
                {
                    widget_dispatch_event(widget, event);
                }
            }

            if (!event->accepted &&
                !window->is_dragging &&
                mouse_event->button == MOUSE_BUTTON_LEFT &&
                rectangle_containe_point(window_header_bound(window), mouse_event->position))
            {
                window->is_dragging = true;
                window_set_cursor(window, CURSOR_MOVE);
            }

            if (!event->accepted &&
                !window->is_resizing &&
                window_resize_bound_containe(window, mouse_event->position))
            {
                window_begin_resize(window, mouse_event->position);
            }
        }

        break;
    }

    case EVENT_MOUSE_BUTTON_RELEASE:
    {
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (window->is_dragging &&
            mouse_event->button == MOUSE_BUTTON_LEFT)
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
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (rectangle_containe_point(widget_bound(window_root(window)), mouse_event->position))
        {
            Widget *widget = widget_child_at(window_root(window), mouse_event->position);

            if (widget)
            {
                widget_dispatch_event(widget, event);
            }
        }

        break;
    }

    case EVENT_KEYBOARD_KEY_TYPED:
    {
        if (window->focused_widget)
        {
            widget_dispatch_event(window->focused_widget, event);
        }
        break;
    }

    default:
        break;
    }
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

void window_set_background(Window *window, Color background)
{
    window->background = background;
}

void window_set_focused_widget(Window *window, Widget *widget)
{
    window->focused_widget = widget;
}

int window_handle(Window *window)
{
    return window->handle;
}

int window_framebuffer_handle(Window *window)
{
    return window->framebuffer_handle;
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
    // FIXME: do something better than this
    list_foreach(Rectangle, rectangle, window->dirty_rect)
    {
        window_paint(window, *rectangle);
        application_blit_window(window, *rectangle);
    }

    list_clear_with_callback(window->dirty_rect, free);
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
            goto merged;
        }
    }

    Rectangle *dirty_rect = __create(Rectangle);

    *dirty_rect = rectangle;

    list_pushback(window->dirty_rect, dirty_rect);

merged:
    return;
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

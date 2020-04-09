#include <libsystem/cstring.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/memory.h>
#include <libwidget/Application.h>
#include <libwidget/Container.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>

#define WINDOW_RESIZE_AREA 16
#define WINDOW_HEADER_AREA 32
#define WINDOW_CONTENT_PADDING 8

struct Window
{
    int handle;
    char *title;

    bool focused;
    bool is_dragging;

    Rectangle on_screen_bound;
    CursorState cursor_state;

    int framebuffer_handle;
    Bitmap *framebuffer;
    Painter *painter;

    List *dirty_rect;
    bool dirty_layout;

    Widget *root_container;
    Widget *focused_widget;

    Color background;

    WindowBorderStyle border;
};

Window *window_create(const char *title, int width, int height)
{
    Window *window = __create(Window);

    static int window_handle_counter = 0;
    window->handle = window_handle_counter++;
    window->title = strdup(title);
    window->focused = false;
    window->cursor_state = CURSOR_DEFAULT;

    window->framebuffer = bitmap_create(width, height);
    window->painter = painter_create(window->framebuffer);
    window->on_screen_bound = RECTANGLE_SIZE(width, height);
    window->dirty_rect = list_create();

    window->root_container = container_create(NULL);
    window->root_container->window = window;
    window->focused_widget = window->root_container;

    shared_memory_get_handle((uintptr_t)window->framebuffer, &window->framebuffer_handle);

    window_layout(window);
    window_update(window, window_bound(window));
    application_add_window(window);

    window->background = THEME_BACKGROUND;
    window->border = WINDOW_BORDER_SIZABLE;

    return window;
}

void window_destroy(Window *window)
{
    widget_destroy(window->root_container);
    free(window->title);
    painter_destroy(window->painter);
    bitmap_destroy(window->framebuffer);
    application_remove_window(window);
    list_destroy_with_callback(window->dirty_rect, free);
    free(window);
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
    if (window->border == WINDOW_BORDER_NONE)
    {
        return window_bound(window);
    }
    else
    {
        return rectangle_shrink(window_bound(window), INSETS(WINDOW_HEADER_AREA, WINDOW_CONTENT_PADDING, WINDOW_CONTENT_PADDING));
    }
}

void window_paint(Window *window, Rectangle rectangle)
{
    painter_clear_rectangle(window->painter, rectangle, window->background);

    if (rectangle_container_rectangle(window_content_bound(window), rectangle))
    {
        if (window_root(window))
        {
            widget_paint(window_root(window), window->painter);
        }
    }
    else
    {

        if (window_root(window))
        {
            widget_paint(window_root(window), window->painter);
        }

        if (window->border != WINDOW_BORDER_NONE)
        {
            if (window->focused)
            {
                painter_fill_rectangle(window->painter, window_header_bound(window), THEME_ALT_BACKGROUND);
            }
            else
            {
                painter_fill_rectangle(window->painter, window_header_bound(window), THEME_BACKGROUND);
            }

            painter_fill_rectangle(window->painter, rectangle_bottom(window_header_bound(window), 1), THEME_ALT_BORDER);
            painter_fill_rectangle(window->painter, rectangle_offset(rectangle_bottom(window_header_bound(window), 1), (Point){0, -1}), THEME_BORDER);

            if (window->focused)
            {
                painter_draw_string(window->painter, widget_font(), window->title, (Point){16, 20}, THEME_FOREGROUND);
                painter_draw_rectangle(window->painter, window_bound(window), THEME_ACCENT);
            }
            else
            {
                painter_draw_string(window->painter, widget_font(), window->title, (Point){16, 20}, THEME_BORDER);
                painter_draw_rectangle(window->painter, window_bound(window), THEME_BORDER);
            }
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
        else
        {
            RectangeBorder borders = rectangle_inset_containe_point(
                rectangle_expand(
                    window_bound_on_screen(window),
                    INSETS(WINDOW_RESIZE_AREA)),
                INSETS(WINDOW_RESIZE_AREA),
                mouse_event->position);

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
        }

        break;
    }

    case EVENT_MOUSE_BUTTON_PRESS:
    {
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (!window->is_dragging &&
            mouse_event->button == MOUSE_BUTTON_LEFT &&
            rectangle_containe_point(window_header_bound_on_screen(window), mouse_event->position) &&
            window->border != WINDOW_BORDER_NONE)
        {
            window->is_dragging = true;
            window_set_cursor(window, CURSOR_MOVE);
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

Rectangle window_bound(Window *window)
{
    Rectangle bound = {};

    bound.size = window->on_screen_bound.size;

    return bound;
}

void window_set_cursor(Window *window, CursorState state)
{
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

void window_set_border_style(Window *window, WindowBorderStyle border_style)
{
    window->border = border_style;
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

Widget *window_root(Window *window)
{
    return window->root_container;
}

void window_update_callback(Window *window)
{
    // FIXME: do something better than this
    list_foreach(Rectangle, rectangle, window->dirty_rect)
    {
        window_paint(window, *rectangle);
    }
    list_clear_with_callback(window->dirty_rect, free);
    application_blit_window(window, window_bound(window));
}

void window_update(Window *window, Rectangle rectangle)
{
    if (list_count(window->dirty_rect) == 0)
    {
        eventloop_run_later((RunLaterCallback)window_update_callback, window);
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

void window_layout_callback(Window *window)
{
    window_root(window)->bound = window_content_bound(window);
    widget_layout(window_root(window));
}

void window_layout(Window *window)
{
    if (window->dirty_layout)
        return;

    window->dirty_layout = true;

    eventloop_run_later((RunLaterCallback)window_layout_callback, window);
}

bool window_is_focused(Window *window)
{
    return window->focused;
}
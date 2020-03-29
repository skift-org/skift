#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/memory.h>
#include <libwidget/core/Application.h>
#include <libwidget/core/Event.h>
#include <libwidget/core/Theme.h>
#include <libwidget/core/Window.h>

static int _window_id = 0;

void window_set_main_widget(Window *window, Widget *widget)
{
    window->main_widget = widget;
}

Rectangle window_header_bound(Window *window)
{
    return rectangle_set_height(window_bound(window), 32);
}

Rectangle window_header_bound_on_screen(Window *window)
{
    return rectangle_set_height(window_bound_on_screen(window), 32);
}

Rectangle window_drag_bound_on_screen(Window *window)
{
    return rectangle_clip(window_header_bound_on_screen(window), rectangle_shrink(window_bound_on_screen(window), (Spacing){8, 8, 8, 8}));
}

void window_paint(Window *window)
{
    painter_fill_rectangle(window->painter, window_bound(window), THEME_BACKGROUND);

    if (window->focused)
    {
        painter_fill_rectangle(window->painter, window_header_bound(window), THEME_ALT_BACKGROUND);
    }

    painter_fill_rectangle(window->painter, rectangle_bottom(window_header_bound(window), 1), THEME_BORDER);

    if (window->focused)
    {
        painter_fill_rectangle(window->painter, rectangle_offset(rectangle_bottom(window_header_bound(window), 1), (Point){0, 1}), THEME_ALT_BORDER);
        painter_draw_rectangle(window->painter, window_bound(window), THEME_ACCENT);
    }
    else
    {
        painter_draw_rectangle(window->painter, window_bound(window), THEME_BORDER);
    }
}

void window_dump(Window *window)
{
    printf("Window(0x%p) (%d, %d) %dx%d\n",
           window,
           window->bound.X,
           window->bound.Y,
           window->bound.width,
           window->bound.height);

    widget_dump(window->main_widget, 1);
}

void window_handle_event(Window *window, Event *event)
{
    switch (event->type)
    {
    case EVENT_PAINT:
        window_paint(window);
        application_blit_window(window, window->bound);
        break;

    case EVENT_GOT_FOCUS:
        window->focused = true;
        window_handle_event(window, EVENT_NO_ARGS(EVENT_PAINT));
        break;

    case EVENT_LOST_FOCUS:
        window->focused = false;
        window_handle_event(window, EVENT_NO_ARGS(EVENT_PAINT));
        break;

    case EVENT_MOUSE_MOVE:
    {
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (window->is_dragging)
        {
            Point offset = point_sub(mouse_event->position, mouse_event->old_position);
            window->bound = rectangle_offset(window->bound, offset);
            application_move_window(window, window->bound.position);
        }

        break;
    }

    case EVENT_MOUSE_ENTER:
        logger_info("Mouse enter ");
        break;

    case EVENT_MOUSE_LEAVE:
        logger_info("Mouse leave ");
        break;

    case EVENT_MOUSE_BUTTON_PRESS:
    {
        logger_info("Mouse press ");
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (!window->is_dragging &&
            mouse_event->button == MOUSE_BUTTON_LEFT &&
            rectangle_containe_point(window_drag_bound_on_screen(window), mouse_event->position))
        {
            window->is_dragging = true;
            window_set_cursor(window, CURSOR_MOVE);
        }

        break;
    }

    case EVENT_MOUSE_BUTTON_RELEASE:
    {
        logger_info("Mouse release ");
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (window->is_dragging &&
            mouse_event->button == MOUSE_BUTTON_LEFT)
        {
            window->is_dragging = false;
            window_set_cursor(window, CURSOR_DEFAULT);
        }

        break;
    }

    default:
        break;
    }
}

Window *window_create(Rectangle bound)
{
    Window *window = __create(Window);

    window->id = _window_id++;
    window->focused = false;
    window->cursor_state = CURSOR_DEFAULT;

    window->framebuffer = bitmap_create(bound.width, bound.height);
    window->painter = painter_create(window->framebuffer);
    window->bound = bound;

    shared_memory_get_handle((uintptr_t)window->framebuffer, &window->framebuffer_handle);

    window_paint(window);
    application_add_window(window);

    return window;
}

void window_destroy(Window *window)
{
    application_remove_window(window);
}

Rectangle window_bound_on_screen(Window *window)
{
    return window->bound;
}

Rectangle window_bound(Window *window)
{
    Rectangle bound = {};

    bound.size = window->bound.size;

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

#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/memory.h>
#include <libwidget/core/Application.h>
#include <libwidget/core/Event.h>
#include <libwidget/core/Theme.h>
#include <libwidget/core/Window.h>

#define WINDOW_RESIZE_AREA 16
#define WINDOW_HEADER_AREA 32
#define WINDOW_CONTENT_PADDING 8

static int _window_id = 0;

static Font *_title_font = NULL;
Font *window_title_font(void)
{
    if (_title_font == NULL)
    {
        _title_font = font_create("sans");
    }

    return _title_font;
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
    return rectangle_shrink(window_bound(window), INSETS(WINDOW_HEADER_AREA, WINDOW_CONTENT_PADDING, WINDOW_CONTENT_PADDING));
}

void window_paint(Window *window)
{
    painter_clear_rectangle(window->painter, window_bound(window), THEME_BACKGROUND);

    if (window->focused)
    {
        painter_fill_rectangle(window->painter, window_header_bound(window), THEME_ALT_BACKGROUND);
    }

    painter_fill_rectangle(window->painter, rectangle_bottom(window_header_bound(window), 1), THEME_BORDER);

    if (window->focused)
    {
        painter_fill_rectangle(window->painter, rectangle_offset(rectangle_bottom(window_header_bound(window), 1), (Point){0, 1}), THEME_ALT_BORDER);
        painter_draw_rectangle(window->painter, window_bound(window), THEME_ACCENT);
        painter_draw_string(window->painter, window_title_font(), window->title, (Point){16, 20}, THEME_FOREGROUND);
    }
    else
    {
        painter_draw_rectangle(window->painter, window_bound(window), THEME_BORDER);
        painter_draw_string(window->painter, window_title_font(), window->title, (Point){16, 20}, THEME_BORDER);
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
        window_paint(window);
        application_blit_window(window, window->on_screen_bound);
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
        logger_info("Mouse press ");
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (!window->is_dragging &&
            mouse_event->button == MOUSE_BUTTON_LEFT &&
            rectangle_containe_point(window_header_bound_on_screen(window), mouse_event->position))
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

Window *window_create(const char *title, int width, int height)
{
    Window *window = __create(Window);

    window->id = _window_id++;
    window->title = strdup(title);
    window->focused = false;
    window->cursor_state = CURSOR_DEFAULT;

    window->framebuffer = bitmap_create(width, height);
    window->painter = painter_create(window->framebuffer);
    window->on_screen_bound = RECTANGLE_SIZE(width, height);

    shared_memory_get_handle((uintptr_t)window->framebuffer, &window->framebuffer_handle);

    window_paint(window);
    application_add_window(window);

    return window;
}

void window_destroy(Window *window)
{
    free(window->title);
    painter_destroy(window->painter);
    application_remove_window(window);
    free(window);
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

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

void window_paint(Window *window)
{
    painter_fill_rectangle(window->painter, window->bound, THEME_BACKGROUND);
    painter_fill_rectangle(window->painter, rectangle_set_height(window->bound, 32), THEME_ALT_BACKGROUND);
    painter_draw_rectangle(window->painter, rectangle_set_height(window->bound, 32), THEME_BORDER);
    painter_draw_rectangle(window->painter, window->bound, THEME_ACCENT);
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
    case EVENT_MOUSE_MOVE:
    {
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (mouse_event->buttons & MOUSE_BUTTON_LEFT)
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

    case EVENT_MOUSE_BUTTON_RELEASE:
        logger_info("Mouse release ");

        break;

    case EVENT_MOUSE_BUTTON_PRESS:
        logger_info("Mouse press ");

        break;

    default:
        break;
    }
}

Window *window_create(Rectangle bound)
{
    Window *window = __create(Window);

    window->id = _window_id++;

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
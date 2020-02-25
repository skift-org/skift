#include <libsystem/logger.h>
#include <libsystem/memory.h>
#include <libwidget/core/Application.h>
#include <libwidget/core/Event.h>
#include <libwidget/core/Theme.h>
#include <libwidget/core/Window.h>

void window_destroy(Window *window)
{
    application_remove_window(window);
}

void window_paint(Window *window, Painter *painter)
{
    painter_fill_rectangle(painter, WIDGET(window)->bound, THEME_BACKGROUND);
    painter_fill_rectangle(painter, rectangle_set_height(WIDGET(window)->bound, 32), THEME_ALT_BACKGROUND);
    painter_draw_rectangle(painter, rectangle_set_height(WIDGET(window)->bound, 32), THEME_BORDER);
    painter_draw_rectangle(painter, WIDGET(window)->bound, THEME_ACCENT);
}

void window_event(Window *window, Event *event)
{
    switch (event->type)
    {
    case EVENT_PAINT:
        widget_paint(WIDGET(window), window->painter);
        application_blit_window(window, WIDGET(window)->bound);
        break;

    default:
        break;
    }
}

static int _window_id = 0;

Widget *window_create(Rectangle bound)
{
    Window *window = __create(Window);

    WIDGET(window)->destroy = (WidgetDestroyCallback)window_destroy;
    WIDGET(window)->paint = (WidgetPaintCallback)window_paint;
    WIDGET(window)->event = (WidgetEventCallback)window_event;

    widget_initialize(WIDGET(window), "Window", NULL, bound);

    window->id = _window_id++;

    window->framebuffer = bitmap_create(bound.width, bound.height);
    window->painter = painter_create(window->framebuffer);

    shared_memory_get_handle((uintptr_t)window->framebuffer, &window->framebuffer_handle);

    application_add_window(window);

    widget_paint(WIDGET(window), window->painter);

    return WIDGET(window);
}

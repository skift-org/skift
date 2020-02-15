#include <libwidget/core/Application.h>
#include <libwidget/core/Window.h>

void window_WidgetDestroyCallback(Window *window)
{
    application_remove_window(window);
}

static int _window_id = 0;

Widget *window_create(Rectangle bound)
{
    Window *window = __create(Window);

    WIDGET(window)->destroy = (WidgetDestroyCallback)window_WidgetDestroyCallback;

    window->id = _window_id++;

    bitmap_create_shared(bound.width, bound.height, &window->framebuffer, &window->framebuffer_handle);
    window->painter = painter_create(window->framebuffer);

    painter_fill_rect(window->painter, bound, COLOR_BLACK);
    painter_draw_rect(window->painter, bound, COLOR_CORNFLOWERBLUE);

    widget_initialize(WIDGET(window), "Window", NULL, bound);
    application_add_window(window);

    return WIDGET(window);
}

#include <libsystem/memory.h>
#include <libwidget/core/Application.h>
#include <libwidget/core/Theme.h>
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

    window->framebuffer = bitmap_create(bound.width, bound.height);
    shared_memory_get_handle((uintptr_t)window->framebuffer, &window->framebuffer_handle);
    window->painter = painter_create(window->framebuffer);

    painter_fill_rectangle(window->painter, bound, THEME_BACKGROUND);

    painter_fill_rectangle(window->painter, rectangle_set_height(bound, 32), THEME_ALT_BACKGROUND);
    painter_draw_rectangle(window->painter, rectangle_set_height(bound, 32), THEME_BORDER);

    painter_draw_rectangle(window->painter, bound, THEME_ACCENT);

    widget_initialize(WIDGET(window), "Window", NULL, bound);
    application_add_window(window);

    return WIDGET(window);
}

#include <libwidget/core/Application.h>
#include <libwidget/core/Window.h>

void window_WidgetDestroyCallback(Window *window)
{
    application_remove_window(window);
}

Widget *window_create(Rectangle bound)
{
    Window *window = __create(Window);

    WIDGET(window)->destroy = (WidgetDestroyCallback)window_WidgetDestroyCallback;

    widget_initialize(WIDGET(window), "Window", NULL, bound);
    application_add_window(window);

    return WIDGET(window);
}

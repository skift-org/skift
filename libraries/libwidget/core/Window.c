#include <libwidget/core/Application.h>
#include <libwidget/core/Window.h>

void window_WidgetDestroyCallback(Window *window)
{
    application_remove_window(window);
}

Widget *window_create(void)
{
    Window *window = __create(Window);

    WIDGET(window)->destroy = (WidgetDestroyCallback)window_WidgetDestroyCallback;

    widget_initialize("Window", WIDGET(window), NULL);
    application_add_window(window);

    return WIDGET(window);
}

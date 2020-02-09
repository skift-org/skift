#include "Compositor/Window.h"
#include "Compositor/Manager.h"

Window *window_create(void)
{
    Window *window = __create(Window);

    manager_register_window(window);

    return window;
}

void window_destroy(Window *window)
{
    manager_unregister_window(window);

    free(window);
}

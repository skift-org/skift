#include "Compositor/Window.h"
#include "Compositor/Manager.h"
#include "Compositor/Renderer.h"

Window *window_create(Rectangle bound)
{
    Window *window = __create(Window);

    manager_register_window(window);

    window->bound = bound;

    renderer_region(window->bound);

    return window;
}

void window_destroy(Window *window)
{
    manager_unregister_window(window);

    free(window);
}

Rectangle window_bound(Window *window)
{
    return window->bound;
}

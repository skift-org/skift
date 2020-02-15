#include "Compositor/Window.h"
#include "Compositor/Client.h"
#include "Compositor/Manager.h"
#include "Compositor/Renderer.h"

Window *window_create(int id, Client *client, Rectangle bound, Bitmap *framebuffer)
{
    Window *window = __create(Window);

    manager_register_window(window);

    window->id = id;
    window->client = client;
    window->bound = bound;
    window->framebuffer = framebuffer;

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

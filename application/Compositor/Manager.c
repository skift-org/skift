
#include "Compositor/Manager.h"
#include "Compositor/Renderer.h"
#include "Compositor/Window.h"

static List *_managed_windows;

void manager_initialize(void)
{
    _managed_windows = list_create();
}

List *manager_get_windows(void)
{
    return _managed_windows;
}

Window *manager_get_window(struct Client *client, int id)
{
    list_foreach(Window, window, _managed_windows)
    {
        if (window->client == client && window->id == id)
        {
            return window;
        }
    }

    return NULL;
}

void manager_register_window(Window *window)
{
    list_pushback(_managed_windows, window);
    renderer_region_dirty(window_bound(window));
}

void manager_unregister_window(Window *window)
{
    renderer_region_dirty(window_bound(window));
    list_remove(_managed_windows, window);
}
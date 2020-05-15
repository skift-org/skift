#include <libsystem/logger.h>

#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static List *_managed_windows;
static Window *_focused_window = NULL;

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

Window *manager_get_window_at(Vec2i position)
{
    list_foreach(Window, window, _managed_windows)
    {
        if (rectangle_containe_point(window_cursor_capture_bound(window), position))
        {
            return window;
        }
    }

    return NULL;
}

void manager_register_window(Window *window)
{
    manager_set_focus_window(window);
    renderer_region_dirty(window_bound(window));
}

void manager_unregister_window(Window *window)
{
    renderer_region_dirty(window_bound(window));
    list_remove(_managed_windows, window);

    Window *top_window = NULL;

    if (list_peek(_managed_windows, (void **)&top_window))
    {
        manager_set_focus_window(top_window);
    }
    else
    {
        manager_set_focus_window(NULL);
    }
}

void manager_set_focus_window(Window *window)
{
    if (_focused_window)
    {
        window_lost_focus(_focused_window);
    }

    _focused_window = window;

    if (_focused_window)
    {
        list_remove(_managed_windows, window);
        list_push(_managed_windows, window);

        window_get_focus(window);
    }
}

struct Window *manager_focus_window(void)
{
    return _focused_window;
}

#include <libsystem/Logger.h>

#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static List *_managed_windows;
static Window *_focused_window = nullptr;

void manager_initialize()
{
    _managed_windows = list_create();
}

List *manager_get_windows()
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

    return nullptr;
}

Window *manager_get_window_at(Vec2i position)
{
    list_foreach(Window, window, _managed_windows)
    {
        if (window_cursor_capture_bound(window).containe(position))
        {
            return window;
        }
    }

    return nullptr;
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

    manager_set_focus_window((Window *)list_peek(_managed_windows));
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

struct Window *manager_focus_window()
{
    return _focused_window;
}

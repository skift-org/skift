#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static List<Window *> *_managed_windows;
static Window *_focused_window = nullptr;

void manager_initialize()
{
    _managed_windows = new List<Window *>();
}

List<Window *> &manager_get_windows()
{
    return *_managed_windows;
}

Window *manager_get_window(struct Client *client, int id)
{
    for (auto *window : *_managed_windows)
    {
        if (window->client() == client && window->id() == id)
        {
            return window;
        }
    }

    return nullptr;
}

Window *manager_get_window_at(Math::Vec2i position)
{
    Window *result = nullptr;

    manager_iterate_front_to_back([&](Window *window) {
        if (window->cursor_capture_bound().contains(position) &&
            (window->flags() & WINDOW_NO_FOCUS) == 0)
        {
            result = window;
            return Iteration::STOP;
        }

        return Iteration::CONTINUE;
    });

    return result;
}

void manager_register_window(Window *window)
{
    manager_set_focus_window(window);
    renderer_region_dirty(window->bound());
}

void manager_unregister_window(Window *window)
{
    renderer_region_dirty(window->bound());
    _managed_windows->remove(window);

    manager_set_focus_window(_managed_windows->peek());
}

void manager_set_focus_window(Window *window)
{
    if (_focused_window)
    {
        _focused_window->lost_focus();
    }

    _focused_window = window;

    if (_focused_window)
    {
        _managed_windows->remove(window);
        _managed_windows->push(window);

        window->get_focus();
    }
}

struct Window *manager_focus_window()
{
    return _focused_window;
}

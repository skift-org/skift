#pragma once

#include <libsystem/utils/List.h>
#include <libutils/Rect.h>

#include "compositor/Protocol.h"

struct Client;
struct Window;

void manager_initialize();

List *manager_get_windows();

struct Window *manager_get_window(struct Client *client, int id);

struct Window *manager_get_window_at(Vec2i position);

void manager_register_window(struct Window *window);

void manager_unregister_window(struct Window *window);

void manager_set_focus_window(struct Window *window);

struct Window *manager_focus_window();

#include "compositor/Window.h"

template <typename Callback>
Iteration manager_iterate_by_type_front_to_back(WindowType type, Callback callback)
{
    list_foreach(Window, window, manager_get_windows())
    {
        if (window->type() == type)
        {
            if (callback(window) == Iteration::STOP)
            {
                return Iteration::STOP;
            }
        }
    }

    return Iteration::CONTINUE;
}

template <typename Callback>
void manager_iterate_front_to_back(Callback callback)
{
    if (manager_iterate_by_type_front_to_back(WINDOW_TYPE_POPOVER, callback) == Iteration::STOP)
        return;

    if (manager_iterate_by_type_front_to_back(WINDOW_TYPE_PANEL, callback) == Iteration::STOP)
        return;

    if (manager_iterate_by_type_front_to_back(WINDOW_TYPE_REGULAR, callback) == Iteration::STOP)
        return;

    if (manager_iterate_by_type_front_to_back(WINDOW_TYPE_DESKTOP, callback) == Iteration::STOP)
        return;
}

template <typename Callback>
Iteration manager_iterate_by_type_back_to_front(WindowType type, Callback callback)
{
    list_foreach_reversed(Window, window, manager_get_windows())
    {
        if (window->type() == type)
        {
            if (callback(window) == Iteration::STOP)
            {
                return Iteration::STOP;
            }
        }
    }

    return Iteration::CONTINUE;
}

template <typename Callback>
void manager_iterate_back_to_front(Callback callback)
{
    if (manager_iterate_by_type_back_to_front(WINDOW_TYPE_DESKTOP, callback) == Iteration::STOP)
        return;

    if (manager_iterate_by_type_back_to_front(WINDOW_TYPE_REGULAR, callback) == Iteration::STOP)
        return;

    if (manager_iterate_by_type_back_to_front(WINDOW_TYPE_PANEL, callback) == Iteration::STOP)
        return;

    if (manager_iterate_by_type_back_to_front(WINDOW_TYPE_POPOVER, callback) == Iteration::STOP)
        return;
}

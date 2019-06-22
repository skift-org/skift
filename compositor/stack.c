/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "compositor/stack.h"

hideo_stack_t *hideo_stack(void)
{
    hideo_stack_t *this = OBJECT(hideo_stack);

    this->windows = list();
    this->focused = NULL;

    return this;
}

void hideo_stack_delete(hideo_stack_t *this)
{
    list_delete(this->windows, LIST_RELEASE_VALUES);
}

void hideo_stack_add_window(hideo_stack_t *this, hideo_window_t *win)
{
    if (!list_containe(this->windows, win))
    {
        list_push(this->windows, object_retain(win));

        if (this->focused == NULL)
        {
            this->focused = win;
            hideo_window_set_focused(win, true);
        }
    }
}

void hideo_stack_remove_window(hideo_stack_t *this, hideo_window_t *win)
{
    if (!list_containe(this->windows, win))
    {
        if (this->focused == win)
        {
            hideo_window_t *wintop;
            if (list_pop(this->windows, (void **)&wintop))
            {
                this->focused = wintop;
                hideo_window_set_focused(wintop, true);
            }
        }

        list_remove(this->windows, win);
        object_release(win);
    }
}

hideo_window_t *hideo_stack_window_at(hideo_stack_t *this, point_t at)
{
    list_foreach(i, this->windows)
    {
        hideo_window_t *win = (hideo_window_t *)i->value;

        if (rectangle_containe_position(win->bound, at))
        {
            return win;
        }
    }

    return NULL;
}

hideo_window_t *hideo_stack_window_get_focus(hideo_stack_t *this)
{
    return this->focused;
}

void hideo_stack_window_set_focus(hideo_stack_t *this, hideo_window_t *win)
{
    if (this->focused == win)
        return;

    if (this->focused != NULL)
    {
        hideo_window_set_focused(this->focused, false);
        this->focused = NULL;
    }

    list_remove(this->windows, win);

    hideo_stack_add_window(this, win);
}

void hideo_stack_repaint(hideo_stack_t *this, hideo_renderer_t *render, hideo_assets_t *assets)
{
    list_foreachR(i, this->windows)
    {
        hideo_window_t *win = (hideo_window_t *)i->value;
        hideo_window_repaint(win, render, assets);
    }
}
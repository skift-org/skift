#include <libgraphic/Bitmap.h>
#include <libsystem/logger.h>

#include "Compositor/Cursor.h"
#include "Compositor/Manager.h"
#include "Compositor/Renderer.h"
#include "Compositor/Window.h"

static Point _mouse_position;
static MouseButton _mouse_buttons;

static Point _mouse_old_position;
static MouseButton _mouse_old_buttons;

static const char *_cursor_paths[] = {
    [CURSOR_BUSY] = "/res/mouse/busy.png",
    [CURSOR_DEFAULT] = "/res/mouse/default.png",
    [CURSOR_DISABLED] = "/res/mouse/disabled.png",
    [CURSOR_MOVE] = "/res/mouse/move.png",
    [CURSOR_RESIZEH] = "/res/mouse/resizeh.png",
    [CURSOR_RESIZEHV] = "/res/mouse/resizehv.png",
    [CURSOR_RESIZEV] = "/res/mouse/resizev.png",
    [CURSOR_RESIZEVH] = "/res/mouse/resizevh.png",
    [CURSOR_TEXT] = "/res/mouse/text.png",
};

static Bitmap *_cursor_bitmaps[__CURSOR_COUNT] = {};

void cursor_initialize(void)
{
    for (size_t i = 0; i < __CURSOR_COUNT; i++)
    {
        _cursor_bitmaps[i] = bitmap_load_from(_cursor_paths[i]);
    }
}

MouseButton cursor_pack_mouse_buttons(MousePacket packet)
{
    MouseButton buttons = MOUSE_NO_BUTTON;

    if (packet.left)
        buttons |= MOUSE_BUTTON_LEFT;

    if (packet.right)
        buttons |= MOUSE_BUTTON_RIGHT;

    if (packet.middle)
        buttons |= MOUSE_BUTTON_LEFT;

    return buttons;
}

Point cursor_pack_mouse_position(MousePacket packet)
{
    return point_clamp_to_rect(
        point_add(
            _mouse_position,
            (Point){packet.offx, packet.offy}),
        renderer_bound());
}

void cursor_handle_packet(MousePacket packet)
{
    _mouse_old_position = _mouse_position;
    _mouse_position = cursor_pack_mouse_position(packet);

    _mouse_old_buttons = _mouse_buttons;
    _mouse_buttons = cursor_pack_mouse_buttons(packet);

    Window *window_under = manager_get_window_at(_mouse_position);
    Window *window_on_focus = manager_focus_window();

    if (!point_equ(_mouse_old_position, _mouse_position))
    {
        renderer_region_dirty(cursor_dirty_bound_from_position(_mouse_old_position));
        renderer_region_dirty(cursor_dirty_bound_from_position(_mouse_position));

        if (window_on_focus)
            window_handle_mouse_move(window_on_focus, _mouse_old_position, _mouse_position, _mouse_buttons);
    }

    if (_mouse_old_buttons != _mouse_buttons)
    {
        if (window_under)
        {
            if (window_under != window_on_focus)
            {
                manager_set_focus_window(window_under);
                window_on_focus = window_under;
            }
        }

        if (window_on_focus)
            window_handle_mouse_buttons(window_on_focus, _mouse_old_buttons, _mouse_buttons, _mouse_position);
    }
}

CursorState cursor_get_state(void)
{
    Window *window = manager_get_window_at(_mouse_position);

    if (window)
    {
        return window->cursor_state;
    }
    else
    {
        return CURSOR_DEFAULT;
    }
}

void cursor_render(Painter *painter)
{
    Bitmap *cursor_bitmap = _cursor_bitmaps[cursor_get_state()];

    painter_blit_bitmap(painter, cursor_bitmap, bitmap_bound(cursor_bitmap), cursor_bound());
}

Rectangle cursor_bound_from_position(Point position)
{
    CursorState state = cursor_get_state();

    Rectangle bound;

    if (state == CURSOR_MOVE ||
        state == CURSOR_RESIZEH ||
        state == CURSOR_RESIZEV ||
        state == CURSOR_RESIZEHV ||
        state == CURSOR_RESIZEVH)
    {
        bound.position = point_add(position, (Point){-14, -14});
    }
    else
    {
        bound.position = point_add(position, (Point){-2, -2});
    }

    bound.size = (Point){28, 28};

    return bound;
}

Rectangle cursor_bound(void)
{
    return cursor_bound_from_position(_mouse_position);
}

Rectangle cursor_dirty_bound_from_position(Point position)
{
    Rectangle bound;

    bound.position = point_add(position, (Point){-28, -28});

    bound.size = (Point){56, 56};

    return bound;
}
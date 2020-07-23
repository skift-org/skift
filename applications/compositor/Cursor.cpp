#include <libgraphic/Bitmap.h>
#include <libsystem/io/Path.h>
#include <libsystem/Logger.h>
#include <libsystem/system/System.h>

#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static Vec2i _mouse_position;
static MouseButton _mouse_buttons;

static Vec2i _mouse_old_position;
static MouseButton _mouse_old_buttons;

static Bitmap *_cursor_bitmaps[__CURSOR_COUNT] = {};

static uint _last_click = 0;

void cursor_initialize(void)
{
    const char *cursor_paths[] = {
        [CURSOR_BUSY] = "/System/Cursors/busy.png",
        [CURSOR_DEFAULT] = "/System/Cursors/default.png",
        [CURSOR_DISABLED] = "/System/Cursors/disabled.png",
        [CURSOR_MOVE] = "/System/Cursors/move.png",
        [CURSOR_RESIZEH] = "/System/Cursors/resizeh.png",
        [CURSOR_RESIZEHV] = "/System/Cursors/resizehv.png",
        [CURSOR_RESIZEV] = "/System/Cursors/resizev.png",
        [CURSOR_RESIZEVH] = "/System/Cursors/resizevh.png",
        [CURSOR_TEXT] = "/System/Cursors/text.png",
    };

    for (size_t i = 0; i < __CURSOR_COUNT; i++)
    {
        _cursor_bitmaps[i] = bitmap_load_from(cursor_paths[i]);
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

Vec2i cursor_pack_mouse_position(MousePacket packet)
{
    return vec2i_clamp_to_rect(
        vec2i_add(
            _mouse_position,
            vec2i(packet.offx, packet.offy)),
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

    if (!vec2i_equ(_mouse_old_position, _mouse_position))
    {
        renderer_region_dirty(cursor_dirty_bound_from_position(_mouse_old_position));
        renderer_region_dirty(cursor_dirty_bound_from_position(_mouse_position));

        if (window_on_focus)
            window_handle_mouse_move(window_on_focus, _mouse_old_position, _mouse_position, _mouse_buttons);
    }

    if (_mouse_old_buttons < _mouse_buttons)
    {
        if (window_under != window_on_focus)
        {
            manager_set_focus_window(window_under);
            window_on_focus = window_under;
        }
    }

    if (!(_mouse_old_buttons & MOUSE_BUTTON_LEFT) &&
        (_mouse_buttons & MOUSE_BUTTON_LEFT))
    {
        uint current = system_get_ticks();

        if (current - _last_click < 250 && window_on_focus)
        {
            window_handle_double_click(window_on_focus, _mouse_position);
        }

        _last_click = current;
    }

    if (window_on_focus)
        window_handle_mouse_buttons(window_on_focus, _mouse_old_buttons, _mouse_buttons, _mouse_position);
}

CursorState cursor_get_state(void)
{
    Window *window = manager_focus_window();

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

Rectangle cursor_bound_from_position(Vec2i position)
{
    CursorState state = cursor_get_state();

    Rectangle bound;

    if (state == CURSOR_MOVE ||
        state == CURSOR_RESIZEH ||
        state == CURSOR_RESIZEV ||
        state == CURSOR_RESIZEHV ||
        state == CURSOR_RESIZEVH)
    {
        bound.position = vec2i_add(position, vec2i(-14, -14));
    }
    else
    {
        bound.position = vec2i_add(position, vec2i(-2, -2));
    }

    bound.size = vec2i(28, 28);

    return bound;
}

Rectangle cursor_bound(void)
{
    return cursor_bound_from_position(_mouse_position);
}

Rectangle cursor_dirty_bound_from_position(Vec2i position)
{
    Rectangle bound;

    bound.position = vec2i_add(position, vec2i(-28, -28));

    bound.size = vec2i(56, 56);

    return bound;
}

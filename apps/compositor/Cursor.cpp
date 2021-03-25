#include <libgraphic/Bitmap.h>
#include <libsystem/system/System.h>
#include <libutils/Path.h>

#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static Vec2i _mouse_position;
static MouseButton _mouse_buttons;

static Vec2i _mouse_old_position;
static MouseButton _mouse_old_buttons;

static RefPtr<Bitmap> _cursor_bitmaps[__CURSOR_COUNT] = {};

static Tick _last_click = 0;

void cursor_initialize()
{
    const char *cursor_paths[] = {
        "/Files/Cursors/default.png",
        "/Files/Cursors/text.png",
        "/Files/Cursors/disabled.png",
        "/Files/Cursors/busy.png",
        "/Files/Cursors/move.png",
        "/Files/Cursors/resizeh.png",
        "/Files/Cursors/resizehv.png",
        "/Files/Cursors/resizev.png",
        "/Files/Cursors/resizevh.png",
    };

    for (size_t i = 0; i < __CURSOR_COUNT; i++)
    {
        _cursor_bitmaps[i] = Bitmap::load_from_or_placeholder(cursor_paths[i]);
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

Vec2i vec2i_clamp_to_rect(Vec2i p, Recti rect)
{
    return p.clamped(rect.position(), rect.position() + rect.size());
}

Vec2i cursor_pack_mouse_position(MousePacket packet)
{
    return vec2i_clamp_to_rect(_mouse_position + Vec2i(packet.offx, packet.offy), renderer_bound());
}

void cursor_handle_packet(MousePacket packet)
{
    _mouse_old_position = _mouse_position;
    _mouse_position = cursor_pack_mouse_position(packet);

    _mouse_old_buttons = _mouse_buttons;
    _mouse_buttons = cursor_pack_mouse_buttons(packet);

    Window *window_under = manager_get_window_at(_mouse_position);
    Window *window_on_focus = manager_focus_window();

    if (_mouse_old_position != _mouse_position)
    {
        renderer_region_dirty(cursor_dirty_bound_from_position(_mouse_old_position));
        renderer_region_dirty(cursor_dirty_bound_from_position(_mouse_position));

        if (window_on_focus)
        {
            window_on_focus->handle_mouse_move(_mouse_old_position, _mouse_position, _mouse_buttons);
        }
    }

    if (_mouse_old_buttons < _mouse_buttons)
    {
        if (window_under != window_on_focus)
        {
            manager_set_focus_window(window_under);
            window_on_focus = window_under;
        }
    }

    bool has_send_double_click = false;

    if (!(_mouse_old_buttons & MOUSE_BUTTON_LEFT) &&
        (_mouse_buttons & MOUSE_BUTTON_LEFT))
    {
        Tick current = system_get_ticks();

        if (current - _last_click < 250 && window_on_focus)
        {
            window_on_focus->handle_double_click(_mouse_position);
            has_send_double_click = true;
        }

        _last_click = current;
    }

    if (window_on_focus && !has_send_double_click)
    {
        window_on_focus->handle_mouse_buttons(_mouse_old_buttons, _mouse_buttons, _mouse_position);
    }

    if (window_on_focus && packet.scroll != 0)
    {
        window_on_focus->handle_mouse_scroll(_mouse_position, packet.scroll);
    }
}

CursorState cursor_get_state()
{
    Window *window = manager_focus_window();

    if (window)
    {
        return window->cursor_state();
    }
    else
    {
        return CURSOR_DEFAULT;
    }
}

void cursor_render(Painter &painter)
{
    auto cursor_bitmap = _cursor_bitmaps[cursor_get_state()];

    painter.blit(*cursor_bitmap, cursor_bitmap->bound(), cursor_bound());
}

Recti cursor_bound_from_position(Vec2i position)
{
    CursorState state = cursor_get_state();

    Recti bound(position, Vec2i(28, 28));

    if (state == CURSOR_MOVE ||
        state == CURSOR_RESIZEH ||
        state == CURSOR_RESIZEV ||
        state == CURSOR_RESIZEHV ||
        state == CURSOR_RESIZEVH)
    {
        bound = bound.offset({-14, -14});
    }
    else
    {
        bound = bound.offset({-2, -2});
    }

    return bound;
}

Recti cursor_bound()
{
    return cursor_bound_from_position(_mouse_position);
}

Recti cursor_dirty_bound_from_position(Vec2i position)
{
    return Recti(
        position + Vec2i(-28, -28),
        Vec2i(56, 56));
}

Recti cursor_dirty_bound()
{
    return cursor_dirty_bound_from_position(_mouse_position);
}

Vec2i cursor_position()
{
    return _mouse_position;
}

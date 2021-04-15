#include <libgraphic/Bitmap.h>
#include <libsystem/system/System.h>
#include <libio/Path.h>

#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static Math::Vec2i _mouse_position;
static Widget::MouseButton _mouse_buttons;

static Math::Vec2i _mouse_old_position;
static Widget::MouseButton _mouse_old_buttons;

static RefPtr<Graphic::Bitmap> _cursor_bitmaps[Widget::__CURSOR_COUNT] = {};

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

    for (size_t i = 0; i < Widget::__CURSOR_COUNT; i++)
    {
        _cursor_bitmaps[i] = Graphic::Bitmap::load_from_or_placeholder(cursor_paths[i]);
    }
}

Widget::MouseButton cursor_pack_mouse_buttons(MousePacket packet)
{
    Widget::MouseButton buttons = MOUSE_NO_BUTTON;

    if (packet.left)
        buttons |= MOUSE_BUTTON_LEFT;

    if (packet.right)
        buttons |= MOUSE_BUTTON_RIGHT;

    if (packet.middle)
        buttons |= MOUSE_BUTTON_LEFT;

    return buttons;
}

Math::Vec2i vec2i_clamp_to_rect(Math::Vec2i p, Math::Recti rect)
{
    return p.clamped(rect.position(), rect.position() + rect.size());
}

Math::Vec2i cursor_pack_mouse_position(MousePacket packet)
{
    return vec2i_clamp_to_rect(_mouse_position + Math::Vec2i(packet.offx, packet.offy), renderer_bound());
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

Widget::CursorState cursor_get_state()
{
    Window *window = manager_focus_window();

    if (window)
    {
        return window->cursor_state();
    }
    else
    {
        return Widget::CURSOR_DEFAULT;
    }
}

void cursor_render(Graphic::Painter &painter)
{
    auto cursor_bitmap = _cursor_bitmaps[cursor_get_state()];

    painter.blit(*cursor_bitmap, cursor_bitmap->bound(), cursor_bound());
}

Math::Recti cursor_bound_from_position(Math::Vec2i position)
{
    Widget::CursorState state = cursor_get_state();

    Math::Recti bound(position, Math::Vec2i(28, 28));

    if (state == Widget::CURSOR_MOVE ||
        state == Widget::CURSOR_RESIZEH ||
        state == Widget::CURSOR_RESIZEV ||
        state == Widget::CURSOR_RESIZEHV ||
        state == Widget::CURSOR_RESIZEVH)
    {
        bound = bound.offset({-14, -14});
    }
    else
    {
        bound = bound.offset({-2, -2});
    }

    return bound;
}

Math::Recti cursor_bound()
{
    return cursor_bound_from_position(_mouse_position);
}

Math::Recti cursor_dirty_bound_from_position(Math::Vec2i position)
{
    return Math::Recti(
        position + Math::Vec2i(-28, -28),
        Math::Vec2i(56, 56));
}

Math::Recti cursor_dirty_bound()
{
    return cursor_dirty_bound_from_position(_mouse_position);
}

Math::Vec2i cursor_position()
{
    return _mouse_position;
}

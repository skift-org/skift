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

static Bitmap *_cursor;

void cursor_initialize(void)
{
    _cursor = bitmap_load_from("/res/mouse/default.png");
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

    Window *_mouse_old_window = manager_get_window_at(_mouse_old_position);
    Window *_mouse_window = manager_get_window_at(_mouse_position);

    if (!point_equ(_mouse_old_position, _mouse_position))
    {
        renderer_region_dirty(cursor_bound_from_position(_mouse_old_position));
        renderer_region_dirty(cursor_bound_from_position(_mouse_position));

        if (_mouse_window != _mouse_old_window && _mouse_old_window != NULL)
            window_handle_mouse_move(_mouse_old_window, _mouse_old_position, _mouse_position, _mouse_buttons);

        if (_mouse_window)
            window_handle_mouse_move(_mouse_window, _mouse_old_position, _mouse_position, _mouse_buttons);
    }

    if (_mouse_old_buttons != _mouse_buttons)
    {
        if (_mouse_window)
            window_handle_mouse_buttons(_mouse_window, _mouse_old_buttons, _mouse_buttons, _mouse_position);
    }
}

void cursor_render(Painter *painter)
{
    painter_blit_bitmap(painter, _cursor, bitmap_bound(_cursor), cursor_bound());
}

Rectangle cursor_bound_from_position(Point position)
{
    Rectangle bound;

    bound.position = point_add(position, (Point){-2, -2});
    bound.size = (Point){24, 24};

    return bound;
}

Rectangle cursor_bound(void)
{
    return cursor_bound_from_position(_mouse_position);
}
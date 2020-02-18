#include <libgraphic/Bitmap.h>

#include "Compositor/Cursor.h"
#include "Compositor/Renderer.h"

static Point _mouse_position;
static Bitmap *_cursor;

void cursor_initialize(void)
{
    _cursor = bitmap_load_from("/res/mouse/default.png");
}

void cursor_handle_packet(MousePacket packet)
{
    //logger_info("MousePacket{offx=%d, offy=%d, scroll=%d, left=%d, right=%d, midlle=%d}", packet.offx, packet.offy, packet.scroll, packet.left, packet.right, packet.middle);

    renderer_region_dirty(cursor_bound());

    _mouse_position = point_add(_mouse_position, (Point){packet.offx, packet.offy});
    _mouse_position = point_clamp_to_rect(_mouse_position, renderer_bound());
}

void cursor_render(Painter *painter)
{
    painter_blit_bitmap(painter, _cursor, bitmap_bound(_cursor), cursor_bound());
}

Rectangle cursor_bound(void)
{
    Rectangle bound;

    bound.position = point_add(_mouse_position, (Point){-2, -2});
    bound.size = bitmap_bound(_cursor).size;

    return bound;
}
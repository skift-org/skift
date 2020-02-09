#include <libgraphic/bitmap.h>

#include "Compositor/Cursor.h"
#include "Compositor/Renderer.h"

static int _mouse_x;
static int _mouse_y;
static Bitmap *_cursor;

void cursor_initialize(void)
{
    _cursor = bitmap_load_from("/res/mouse/default.png");
}

void cursor_handle_packet(MousePacket packet)
{
    renderer_region(cursor_bound());

    _mouse_x += packet.offx;
    _mouse_y += packet.offy;
}

void cursor_render(Painter *painter)
{
    painter_blit_bitmap(painter, _cursor, bitmap_bound(_cursor), cursor_bound());
}

Rectangle cursor_bound(void)
{
    return (Rectangle){{_mouse_x, _mouse_y, bitmap_bound(_cursor).width, bitmap_bound(_cursor).height}};
}
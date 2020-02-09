#include <libgraphic/framebuffer.h>

#include "Compositor/Cursor.h"
#include "Compositor/Renderer.h"

static framebuffer_t *_framebuffer;
static Painter *_painter;

void renderer_initialize(void)
{
    _framebuffer = framebuffer_open();
    _painter = _framebuffer->painter;

    renderer_region(framebuffer_bound(_framebuffer));
    renderer_blit();
}

void renderer_region(Rectangle region)
{
    painter_fill_rect(_painter, region, COLOR_DIMGRAY);

    painter_draw_rect(_painter, region, COLOR_MAGENTA);

    framebuffer_mark_dirty(_framebuffer, region);
}

void renderer_blit(void)
{
    renderer_region(cursor_bound());
    cursor_render(_painter);

    framebuffer_blit_dirty(_framebuffer);
}
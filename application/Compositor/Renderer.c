#include <libgraphic/framebuffer.h>

#include "Compositor/Cursor.h"
#include "Compositor/Manager.h"
#include "Compositor/Renderer.h"
#include "Compositor/Window.h"

static framebuffer_t *_framebuffer;
static Painter *_painter;
static Bitmap *_wallpaper;

void renderer_initialize(void)
{
    _framebuffer = framebuffer_open();
    _painter = _framebuffer->painter;
    _wallpaper = bitmap_load_from("/res/wallpaper/brand.png");

    renderer_region(framebuffer_bound(_framebuffer));
    renderer_blit();
}

void renderer_region(Rectangle region)
{
    painter_blit_bitmap(_painter, _wallpaper, region, region);

    list_foreach(Window, window, manager_get_windows())
    {
        if (rectange_colide(window_bound(window), region))
        {
            Rectangle cliped = rectangle_clip(window_bound(window), region);

            painter_fill_rect(_painter, cliped, COLOR_WHITE);
        }
    }

    framebuffer_mark_dirty(_framebuffer, region);
}

Rectangle renderer_bound(void)
{
    return framebuffer_bound(_framebuffer);
}

void renderer_blit(void)
{
    renderer_region(cursor_bound());
    cursor_render(_painter);

    framebuffer_blit_dirty(_framebuffer);
}
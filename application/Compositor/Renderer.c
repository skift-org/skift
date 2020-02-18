#include <libgraphic/Framebuffer.h>

#include "Compositor/Cursor.h"
#include "Compositor/Manager.h"
#include "Compositor/Renderer.h"
#include "Compositor/Window.h"

static Framebuffer *_framebuffer;
static Painter *_painter;
static Bitmap *_wallpaper;
static Rectangle _dirty;

void renderer_initialize(void)
{
    _framebuffer = framebuffer_open();
    _painter = _framebuffer->painter;
    _wallpaper = bitmap_load_from("/res/wallpaper/paint.png");

    renderer_region_dirty(framebuffer_bound(_framebuffer));
}

void renderer_region_dirty(Rectangle region)
{
    if (rectangle_is_empty(_dirty))
    {
        _dirty = region;
    }
    else
    {
        _dirty = rectangle_merge(_dirty, region);
    }
}

void renderer_region(Rectangle region)
{
    painter_blit_bitmap(_painter, _wallpaper, region, region);

    list_foreach(Window, window, manager_get_windows())
    {
        if (rectangle_colide(window_bound(window), region))
        {
            Rectangle cliped = rectangle_clip(window_bound(window), region);

            cliped = rectangle_offset(cliped, point_sub(POINT_ZERO, window->bound.position));

            painter_blit_bitmap(_painter, window->framebuffer, cliped, cliped);
        }
    }

    framebuffer_mark_dirty(_framebuffer, region);
}

Rectangle renderer_bound(void)
{
    return framebuffer_bound(_framebuffer);
}

void renderer_repaint_dirty(void)
{
    if (!rectangle_is_empty(_dirty))
    {
        renderer_region(_dirty);

        if (rectangle_colide(_dirty, cursor_bound()))
        {
            renderer_region(cursor_bound());

            cursor_render(_painter);
        }

        _dirty = RECTANGLE_EMPTY;

        framebuffer_blit_dirty(_framebuffer);
    }
}

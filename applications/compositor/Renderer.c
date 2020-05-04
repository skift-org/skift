#include <libgraphic/Framebuffer.h>

#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static Framebuffer *_framebuffer;
static Painter *_painter;
static Bitmap *_wallpaper;

static List *_dirty_regions = NULL;

void renderer_initialize(void)
{
    _framebuffer = framebuffer_open();
    _painter = _framebuffer->painter;
    _wallpaper = bitmap_load_from("/res/wallpaper/mountains.png");
    _dirty_regions = list_create();

    renderer_region_dirty(framebuffer_bound(_framebuffer));
}

void renderer_region_dirty(Rectangle new_region)
{
    bool merged = false;

    list_foreach(Rectangle, region, _dirty_regions)
    {
        if (rectangle_colide(*region, new_region))
        {
            *region = rectangle_merge(*region, new_region);
            merged = true;
        }
    }

    if (!merged)
    {
        Rectangle *r = __create(Rectangle);
        *r = new_region;

        list_pushback(_dirty_regions, r);
    }
}

void renderer_region(Rectangle region)
{
    painter_blit_bitmap(_painter, _wallpaper, region, region);

    list_foreach_reversed(Window, window, manager_get_windows())
    {
        if (rectangle_colide(window_bound(window), region))
        {
            Rectangle destination = rectangle_clip(window_bound(window), region);

            double x_scale = (double)bitmap_bound(window->frontbuffer).width / window_bound(window).width;
            double y_scale = (double)bitmap_bound(window->frontbuffer).height / window_bound(window).height;

            Rectangle source = {{
                (destination.X - window_bound(window).X) * x_scale,
                (destination.Y - window_bound(window).Y) * y_scale,
                destination.width * x_scale,
                destination.height * y_scale,
            }};

            painter_blit_bitmap(_painter, window->frontbuffer, source, destination);
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
    list_foreach(Rectangle, region, _dirty_regions)
    {
        renderer_region(*region);

        if (rectangle_colide(*region, cursor_bound()))
        {
            renderer_region(cursor_bound());

            cursor_render(_painter);
        }
    }

    framebuffer_blit_dirty(_framebuffer);
    list_clear_with_callback(_dirty_regions, free);
}

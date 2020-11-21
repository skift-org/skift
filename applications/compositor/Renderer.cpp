#include <libgraphic/Framebuffer.h>
#include <libutils/Vector.h>

#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static OwnPtr<Framebuffer> _framebuffer;
static RefPtr<Bitmap> _wallpaper;

static Vector<Rectangle> _dirty_regions;

void renderer_initialize()
{
    _framebuffer = Framebuffer::open().take_value();
    _wallpaper = Bitmap::load_from_or_placeholder("/Files/Wallpapers/mountains.png");

    renderer_region_dirty(_framebuffer->resolution());
}

void renderer_region_dirty(Rectangle new_region)
{
    if (new_region.is_empty())
    {
        return;
    }

    bool merged = false;

    _dirty_regions.foreach ([&](Rectangle &region) {
        if (region.colide_with(new_region))
        {
            Rectangle top;
            Rectangle botton;
            Rectangle left;
            Rectangle right;

            new_region.substract(region, top, botton, left, right);

            renderer_region_dirty(top);
            renderer_region_dirty(botton);
            renderer_region_dirty(left);
            renderer_region_dirty(right);

            merged = true;

            return Iteration::STOP;
        }
        else
        {
            return Iteration::CONTINUE;
        }
    });

    if (!merged)
    {
        _dirty_regions.push_back(new_region);
    }
}

void renderer_composite_wallpaper(Rectangle region)
{
    double scale_x = _wallpaper->width() / (double)_framebuffer->resolution().width();
    double scale_y = _wallpaper->height() / (double)_framebuffer->resolution().height();

    Rectangle source(
        region.x() * scale_x,
        region.y() * scale_y,
        region.width() * scale_x,
        region.height() * scale_y);

    _framebuffer->painter().blit_bitmap_no_alpha(*_wallpaper, source, region);
    _framebuffer->mark_dirty(region);
}

void renderer_composite_region(Rectangle region, Window *window_transparent)
{
    renderer_composite_wallpaper(region);

    manager_iterate_back_to_front([&](Window *window) {
        if (window == window_transparent)
        {
            return Iteration::STOP;
        }

        if (window->bound().colide_with(region))
        {
            Rectangle destination = window->bound().clipped_with(region);

            Rectangle source(
                destination.position() - window->bound().position(),
                destination.size());

            _framebuffer->painter().blit_bitmap(window->frontbuffer(), source, destination);
        }

        return Iteration::CONTINUE;
    });

    _framebuffer->mark_dirty(region);
}

void renderer_region(Rectangle region)
{
    bool should_paint_wallpaper = true;

    if (region.is_empty())
    {
        return;
    }

    manager_iterate_front_to_back([&](Window *window) {
        if (window->bound().colide_with(region))
        {
            Rectangle destination = window->bound().clipped_with(region);

            Rectangle source(
                destination.position() - window->bound().position(),
                destination.size());

            if (window->flags() & WINDOW_TRANSPARENT)
            {
                renderer_composite_region(destination, window);
                //_framebuffer->painter().blur_rectangle(destination, 48);
                _framebuffer->painter().blit_bitmap(window->frontbuffer(), source, destination);
            }
            else
            {
                _framebuffer->painter().blit_bitmap_no_alpha(window->frontbuffer(), source, destination);
            }

            _framebuffer->mark_dirty(destination);

            Rectangle top;
            Rectangle botton;
            Rectangle left;
            Rectangle right;

            region.substract(destination, top, botton, left, right);

            renderer_region(top);
            renderer_region(botton);
            renderer_region(left);
            renderer_region(right);

            should_paint_wallpaper = false;

            return Iteration::STOP;
        }

        return Iteration::CONTINUE;
    });

    if (should_paint_wallpaper)
    {
        renderer_composite_wallpaper(region);
    }
}

Rectangle renderer_bound()
{
    return _framebuffer->resolution();
}

void renderer_repaint_dirty()
{
    _dirty_regions.foreach ([](Rectangle region) {
        renderer_region(region);

        if (region.colide_with(cursor_bound()))
        {
            renderer_region(cursor_bound());

            cursor_render(_framebuffer->painter());
        }

        return Iteration::CONTINUE;
    });

    _framebuffer->blit();

    _dirty_regions.clear();
}

bool renderer_set_resolution(int width, int height)
{
    auto result = _framebuffer->set_resolution(Vec2i(width, height));
    renderer_region_dirty(renderer_bound());
    return result == SUCCESS;
}

void renderer_set_wallaper(RefPtr<Bitmap> wallaper)
{
    _wallpaper = wallaper;
    _wallpaper->filtering(BITMAP_FILTERING_LINEAR);

    renderer_region_dirty(renderer_bound());
}

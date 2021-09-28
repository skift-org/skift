#pragma once

#include <abi/Result.h>
#include <libmath/Rect.h>
#include <libutils/RefPtr.h>
#include <libutils/ResultOr.h>
#include <libutils/String.h>

#include <libgraphic/Color.h>

namespace Graphic
{

enum BitmapStorage
{
    BITMAP_SHARED,
    BITMAP_STATIC,
};

enum BitmapFiltering
{
    NEAREST,
    LINEAR,
};

enum struct BitmapScaling
{
    CENTER,
    STRETCH,
    COVER,
    FIT,
};

struct Bitmap : public RefCounted<Bitmap>
{
private:
    int _handle;
    BitmapStorage _storage;
    int _width;
    int _height;
    BitmapFiltering _filtering;
    Color *_pixels;

    NONCOPYABLE(Bitmap);
    NONMOVABLE(Bitmap);

public:
    Bitmap(int handle, BitmapStorage storage, int width, int height, Color *pixels)
        : _handle(handle),
          _storage(storage),
          _width(width),
          _height(height),
          _filtering(BitmapFiltering::LINEAR),
          _pixels(pixels)
    {
    }

    ~Bitmap();

    Color *pixels() { return _pixels; }

    int handle() const { return _handle; }
    int width() const { return _width; }
    int height() const { return _height; }
    Math::Vec2i size() const { return Math::Vec2i(_width, _height); }
    Math::Recti bound() const { return Math::Recti(_width, _height); }

    void filtering(BitmapFiltering filtering) { _filtering = filtering; }

    static RefPtr<Bitmap> placeholder();

    static ResultOr<RefPtr<Bitmap>> create_shared(int width, int height);

    static ResultOr<RefPtr<Bitmap>> create_shared_from_handle(int handle, Math::Vec2i width_and_height);

    static RefPtr<Bitmap> create_static(int width, int height, Color *pixels);

    static ResultOr<RefPtr<Bitmap>> load_from(String path, int size_hint = -1);

    static RefPtr<Bitmap> load_from_or_placeholder(String path, int size_hint = -1);

    HjResult save_to(String path);

    void set_pixel(Math::Vec2i position, Color color)
    {
        if (bound().contains(position))
        {
            _pixels[position.x() + position.y() * width()] = color;
        }
    }

    void set_pixel_no_check(Math::Vec2i position, Color color)
    {
        _pixels[position.x() + position.y() * width()] = color;
    }

    void blend_pixel(Math::Vec2i position, Color color)
    {
        Color background = get_pixel(position);
        set_pixel(position, Color::blend(color, background));
    }

    void blend_pixel_no_check(Math::Vec2i position, Color color)
    {
        Color background = get_pixel_no_check(position);
        set_pixel_no_check(position, Color::blend(color, background));
    }

    Color get_pixel(Math::Vec2i position)
    {
        return _pixels[clamp(position.y(), 0, height() - 1) * width() + clamp(position.x(), 0, width() - 1)];
    }

    Color get_pixel_no_check(Math::Vec2i position)
    {
        return _pixels[position.y() * width() + position.x()];
    }

    FLATTEN Color sample(Math::Vec2f position)
    {
        return sample(bound(), position);
    }

    FLATTEN Color sample(Math::Recti source, Math::Vec2f position)
    {
        Math::Vec2i sample_position = source.position() + (source.size() * position);

        if (_filtering == BitmapFiltering::NEAREST)
        {
            return get_pixel(source.position() + sample_position);
        }

        Color c00 = get_pixel(sample_position + Math::Vec2i(0, 0));
        Color c10 = get_pixel(sample_position + Math::Vec2i(1, 0));
        Color c01 = get_pixel(sample_position + Math::Vec2i(0, 1));
        Color c11 = get_pixel(sample_position + Math::Vec2i(1, 1));

        float xx = source.width() * position.x();
        float yy = source.height() * position.y();

        return Color::blerp(c00, c10, c01, c11, xx - (int)xx, yy - (int)yy);
    }

    FLATTEN void copy_from(Bitmap &source, Math::Recti region)
    {
        region = region.clipped_with(source.bound());
        region = region.clipped_with(bound());

        if (region.is_empty())
        {
            return;
        }

        for (int y = region.y(); y < region.y() + region.height(); y++)
        {
            for (int x = region.x(); x < region.x() + region.width(); x++)
            {
                set_pixel_no_check(Math::Vec2i(x, y), source.get_pixel_no_check(Math::Vec2i(x, y)));
            }
        }
    }

    void clear(Color color)
    {
        for (int i = 0; i < width() * height(); i++)
        {
            pixels()[i] = color;
        }
    }
};

} // namespace Graphic

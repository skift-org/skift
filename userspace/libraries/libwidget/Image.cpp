#include <libgraphic/Painter.h>
#include <libwidget/Image.h>

namespace Widget
{

Image::Image(RefPtr<Graphic::Bitmap> bitmap)
    : _bitmap(bitmap)
{
}

Image::Image(RefPtr<Graphic::Bitmap> bitmap, Graphic::BitmapScaling scaling)
    : _bitmap(bitmap), _scaling(scaling)
{
}

void Image::change_bitmap(RefPtr<Graphic::Bitmap> bitmap)
{
    if (_bitmap != bitmap)
    {
        _bitmap = bitmap;
        should_repaint();
    }
}

void Image::scaling(Graphic::BitmapScaling scaling)
{
    if (_scaling != scaling)
    {
        _scaling = scaling;
        should_repaint();
    }
}

void Image::paint(Graphic::Painter &painter, const Math::Recti &)
{
    if (!_bitmap)
    {
        return;
    }

    painter.blit(*_bitmap, _scaling, bound());
}

Math::Vec2i Image::size()
{
    if (_scaling == Graphic::BitmapScaling::CENTER)
    {
        return _bitmap->bound().size();
    }

    return {};
}

} // namespace Widget

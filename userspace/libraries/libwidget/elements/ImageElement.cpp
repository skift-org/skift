#include <libgraphic/Painter.h>
#include <libwidget/elements/ImageElement.h>

namespace Widget
{

ImageElement::ImageElement(RefPtr<Graphic::Bitmap> bitmap)
    : _bitmap(bitmap)
{
}

ImageElement::ImageElement(RefPtr<Graphic::Bitmap> bitmap, Graphic::BitmapScaling scaling)
    : _bitmap(bitmap), _scaling(scaling)
{
}

void ImageElement::change_bitmap(RefPtr<Graphic::Bitmap> bitmap)
{
    if (_bitmap != bitmap)
    {
        _bitmap = bitmap;
        should_repaint();
    }
}

void ImageElement::scaling(Graphic::BitmapScaling scaling)
{
    if (_scaling != scaling)
    {
        _scaling = scaling;
        should_repaint();
    }
}

void ImageElement::paint(Graphic::Painter &painter, const Math::Recti &)
{
    if (!_bitmap)
    {
        return;
    }

    painter.blit(*_bitmap, _scaling, bound());
}

Math::Vec2i ImageElement::size()
{
    if (_scaling == Graphic::BitmapScaling::CENTER)
    {
        return _bitmap->bound().size();
    }

    return {};
}

} // namespace Widget

#include <libgraphic/Painter.h>
#include <libwidget/elements/ImageElement.h>

namespace Widget
{

ImageElement::ImageElement(RefPtr<Graphic::Bitmap> bitmap, Graphic::BitmapScaling scaling)
    : _bitmap(bitmap), _scaling(scaling)
{
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

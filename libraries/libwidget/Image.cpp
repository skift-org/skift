#include <libgraphic/Painter.h>
#include <libwidget/Image.h>

Image::Image(Widget *parent, RefPtr<Bitmap> bitmap)
    : Widget(parent), _bitmap(bitmap)
{
}

Image::Image(Widget *parent, RefPtr<Bitmap> bitmap, BitmapScaling scaling)
    : Widget(parent), _bitmap(bitmap), _scaling(scaling)
{
}

void Image::change_bitmap(RefPtr<Bitmap> bitmap)
{
    if (_bitmap != bitmap)
    {
        _bitmap = bitmap;
        should_repaint();
    }
}

void Image::scaling(BitmapScaling scaling)
{
    if (_scaling != scaling)
    {
        _scaling = scaling;
        should_repaint();
    }
}

void Image::paint(Painter &painter, const Recti &)
{
    if (!_bitmap)
    {
        return;
    }

    painter.blit(*_bitmap, _scaling, bound());
}

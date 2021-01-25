#include <libgraphic/Painter.h>
#include <libwidget/Image.h>

Image::Image(Widget *parent, RefPtr<Bitmap> bitmap)
    : Widget(parent), _bitmap(bitmap)
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
    if (_scalling != scaling)
    {
        _scalling = scaling;
        should_repaint();
    }
}

void Image::paint(Painter &painter, const Recti &)
{
    if (!_bitmap)
    {
        return;
    }

    Recti destination = bound();

    painter.blit(*_bitmap, _scalling, destination);
}


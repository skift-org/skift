#pragma once

#include <libwidget/Widget.h>

enum class ImageScalling
{
    CENTER,
    STRETCH,
};

class Image : public Widget
{
private:
    RefPtr<Bitmap> _bitmap;
    ImageScalling _scalling = ImageScalling::CENTER;

public:
    Image(Widget *parent, RefPtr<Bitmap> bitmap);

    void change_bitmap(RefPtr<Bitmap> bitmap);

    void change_scaling(ImageScalling scaling);

    void paint(Painter &, const Recti &) override;

    Vec2i size() override;
};

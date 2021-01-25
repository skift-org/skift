#pragma once

#include <libwidget/Widget.h>

class Image : public Widget
{
private:
    RefPtr<Bitmap> _bitmap;
    BitmapScaling _scalling = BitmapScaling::FIT;

public:
    Image(Widget *parent, RefPtr<Bitmap> bitmap);

    void change_bitmap(RefPtr<Bitmap> bitmap);

    void scaling(BitmapScaling scaling);

    void paint(Painter &, const Recti &) override;
};

#pragma once

#include <libwidget/Widget.h>

class Image : public Widget
{
private:
    RefPtr<Bitmap> _bitmap;
    BitmapScaling _scaling = BitmapScaling::FIT;

public:
    Image(Widget *parent, RefPtr<Bitmap> bitmap);

    Image(Widget *parent, RefPtr<Bitmap> bitmap, BitmapScaling scaling);

    void change_bitmap(RefPtr<Bitmap> bitmap);

    void scaling(BitmapScaling scaling);

    void paint(Painter &, const Recti &) override;

    virtual Vec2i size() override;
};

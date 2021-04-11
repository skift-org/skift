#pragma once

#include <libwidget/Component.h>

namespace Widget
{

class Image : public Component
{
private:
    RefPtr<Graphic::Bitmap> _bitmap;
    Graphic::BitmapScaling _scaling = Graphic::BitmapScaling::FIT;

public:
    Image(Component *parent, RefPtr<Graphic::Bitmap> bitmap);

    Image(Component *parent, RefPtr<Graphic::Bitmap> bitmap, Graphic::BitmapScaling scaling);

    void change_bitmap(RefPtr<Graphic::Bitmap> bitmap);

    void scaling(Graphic::BitmapScaling scaling);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    virtual Math::Vec2i size() override;
};

} // namespace Widget

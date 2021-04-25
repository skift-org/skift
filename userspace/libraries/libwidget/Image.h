#pragma once

#include <libwidget/Element.h>

namespace Widget
{

class Image : public Element
{
private:
    RefPtr<Graphic::Bitmap> _bitmap;
    Graphic::BitmapScaling _scaling = Graphic::BitmapScaling::FIT;

public:
    Image(RefPtr<Graphic::Bitmap> bitmap);

    Image(RefPtr<Graphic::Bitmap> bitmap, Graphic::BitmapScaling scaling);

    void change_bitmap(RefPtr<Graphic::Bitmap> bitmap);

    void scaling(Graphic::BitmapScaling scaling);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    virtual Math::Vec2i size() override;
};

static inline RefPtr<Image> image(
    RefPtr<Graphic::Bitmap> bitmap,
    Graphic::BitmapScaling scaling = Graphic::BitmapScaling::FIT)
{
    return make<Image>(bitmap, scaling);
}

} // namespace Widget

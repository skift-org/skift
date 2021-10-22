#pragma once

#include <libgraphic/Bitmap.h>
#include <libwidget/Element.h>

namespace MediaPlayer
{

struct Cover : public Widget::Element
{
private:
    Ref<Graphic::Bitmap> _cover;
    Ref<Graphic::Bitmap> _backdrop;

public:
    Cover(Ref<Graphic::Bitmap> bitmap);

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

} // namespace MediaPlayer

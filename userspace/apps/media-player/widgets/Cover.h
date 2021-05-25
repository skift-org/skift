#pragma once

#include <libgraphic/Bitmap.h>
#include <libwidget/Element.h>

namespace MediaPlayer
{

struct Cover : public Widget::Element
{
private:
    RefPtr<Graphic::Bitmap> _cover;
    RefPtr<Graphic::Bitmap> _backdrop;

public:
    Cover(RefPtr<Graphic::Bitmap> bitmap);

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

} // namespace MediaPlayer

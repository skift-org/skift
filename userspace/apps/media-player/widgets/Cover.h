#pragma once

#include <libgraphic/Bitmap.h>
#include <libwidget/Component.h>

namespace MediaPlayer
{

class Cover : public Widget::Component
{
private:
    RefPtr<Graphic::Bitmap> _cover;
    RefPtr<Graphic::Bitmap> _backdrop;

public:
    Cover(Widget::Component *parent, RefPtr<Graphic::Bitmap> bitmap);

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

} // namespace MediaPlayer

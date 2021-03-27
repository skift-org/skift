#pragma once

#include <libgraphic/Bitmap.h>
#include <libwidget/Widget.h>

namespace media_player
{

class Cover : public Widget
{
private:
    RefPtr<Graphic::Bitmap> _cover;
    RefPtr<Graphic::Bitmap> _backdrop;

public:
    Cover(Widget *parent, RefPtr<Graphic::Bitmap> bitmap);

    void paint(Graphic::Painter &painter, const Recti &) override;
};

} // namespace media_player

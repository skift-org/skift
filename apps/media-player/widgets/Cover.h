#pragma once

#include <libgraphic/Bitmap.h>
#include <libwidget/Widget.h>

namespace media_player
{

class Cover : public Widget
{
private:
    RefPtr<Bitmap> _cover;
    RefPtr<Bitmap> _backdrop;

public:
    Cover(Widget *parent, RefPtr<Bitmap> bitmap);

    void paint(Painter &painter, const Recti &) override;
};

} // namespace media_player

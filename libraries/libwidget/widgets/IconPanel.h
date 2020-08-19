#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

class IconPanel : public Widget
{
private:
    RefPtr<Icon> _icon;

public:
    IconPanel(Widget *parent, RefPtr<Icon> icon);

    void paint(Painter &painter, Rectangle rectangle);

    Vec2i size();
};

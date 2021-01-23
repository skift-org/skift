#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

class IconPanel : public Widget
{
private:
    RefPtr<Icon> _icon;
    IconSize _icon_size = ICON_18PX;

public:
    void icon_size(IconSize size) { _icon_size = size; }

    IconPanel(Widget *parent, RefPtr<Icon> icon);

    void paint(Painter &, const Recti &) override;

    Vec2i size() override;
};

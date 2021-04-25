#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Element.h>

namespace Widget
{

class IconPanel : public Element
{
private:
    RefPtr<Graphic::Icon> _icon;
    Graphic::IconSize _icon_size = Graphic::ICON_18PX;

public:
    void icon_size(Graphic::IconSize size) { _icon_size = size; }

    IconPanel(RefPtr<Graphic::Icon> icon);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

} // namespace Widget

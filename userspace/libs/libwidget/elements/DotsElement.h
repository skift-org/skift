#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct DotsElement : public Element
{
private:
    const int _count;
    const int _index;

public:
    static constexpr int DOTSIZE = 4;
    static constexpr int DOTSPACING = 8;

    DotsElement(int count, int index);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

WIDGET_BUILDER(DotsElement, dots);

} // namespace  Widget

#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct SeparatorElement : public Element
{
public:
    void paint(Graphic::Painter &painter, const Math::Recti &dirty) override;

    Math::Vec2i size() override;
};

static inline RefPtr<SeparatorElement> separator() { return make<SeparatorElement>(); }

} // namespace Widget

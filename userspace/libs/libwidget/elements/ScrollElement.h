#pragma once

#include <libwidget/elements/ScrollBarElement.h>

namespace Widget
{

struct ScrollElement : public Element
{
private:
    bool _horizontal = false;
    Ref<Element> _host = nullptr;
    Ref<ScrollBarElement> _scrollbar = nullptr;

public:
    Ref<Element> host() { return _host; }

    ScrollElement(Ref<Element> content = nullptr, bool horizontal = false);

    void layout() override;

    void event(Event *event) override;

    Math::Vec2i size() override;
};

WIDGET_BUILDER(ScrollElement, scroll)

} // namespace Widget

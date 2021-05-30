#pragma once

#include <libwidget/elements/ScrollBarElement.h>

namespace Widget
{

struct ScrollElement : public Element
{
private:
    bool _horizontal = false;
    RefPtr<Element> _host = nullptr;
    RefPtr<ScrollBarElement> _scrollbar = nullptr;

public:
    RefPtr<Element> host() { return _host; }

    ScrollElement(RefPtr<Element> content = nullptr, bool horizontal = false);

    void layout() override;

    void event(Event *event) override;

    Math::Vec2i size() override;
};

WIDGET_BUILDER(ScrollElement, scroll)

} // namespace Widget

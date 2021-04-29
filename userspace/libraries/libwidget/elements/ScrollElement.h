#pragma once

#include <libwidget/elements/ScrollBarElement.h>

namespace Widget
{

class ScrollElement : public Element
{
private:
    bool _horizontal = false;
    RefPtr<Element> _host = nullptr;
    RefPtr<ScrollBarElement> _scrollbar = nullptr;

public:
    RefPtr<Element> host() { return _host; }

    ScrollElement(RefPtr<Element> content = nullptr, bool horizontal = false);

    void do_layout() override;

    void event(Event *event) override;

    Math::Vec2i size() override;
};

static inline RefPtr<ScrollElement> scroll(RefPtr<Element> content = nullptr, bool horizontal = false)
{
    return make<ScrollElement>(content, horizontal);
}

} // namespace Widget

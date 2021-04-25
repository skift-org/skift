#pragma once

#include <libwidget/Element.h>
#include <libwidget/ScrollBar.h>

namespace Widget
{

class VScroll : public Element
{
private:
    RefPtr<Element> _host = nullptr;
    RefPtr<ScrollBar> _scrollbar = nullptr;

public:
    RefPtr<Element> host() { return _host; }

    VScroll(RefPtr<Element> content = nullptr)
    {
        if (content != nullptr)
        {
            _host = add(content);
        }
        else
        {
            _host = add<Element>();
        }

        _scrollbar = add<ScrollBar>();
        _scrollbar->flags(Element::NOT_AFFECTED_BY_SCROLL);

        _scrollbar->on(Event::VALUE_CHANGE, [this](auto) {
            scroll({scroll().x(), _scrollbar->value()});
            should_repaint();
        });
    }

    ~VScroll()
    {
    }

    void do_layout() override
    {
        int content_height = _host->size().y();

        _host->container(content().take_top(content_height));
        _scrollbar->container(content().take_right(ScrollBar::SIZE));
        _scrollbar->update(content_height, content().height(), scroll().x());
    }

    void event(Event *event) override
    {
        if (event->type == Event::MOUSE_SCROLL)
        {
            event->accepted = true;
            _scrollbar->dispatch_event(event);
        }
    }

    Math::Vec2i size() override
    {
        return {_host->size().x(), 0};
    }
};

static inline RefPtr<VScroll> vscroll(RefPtr<Element> content = nullptr) { return make<VScroll>(content); }

} // namespace Widget

#pragma once

#include <libwidget/Container.h>
#include <libwidget/ScrollBar.h>

namespace Widget
{

class HScroll : public Element
{
private:
    RefPtr<Element> _host = nullptr;
    RefPtr<ScrollBar> _scrollbar = nullptr;

public:
    RefPtr<Element> host() { return _host; }

    HScroll(RefPtr<Element> content = nullptr)
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
            scroll({_scrollbar->value(), scroll().y()});
            should_repaint();
        });
    }

    void do_layout() override
    {
        int content_width = _host->size().x();

        _host->container(content().take_left(content_width));
        _scrollbar->container(content().take_bottom(ScrollBar::SIZE));
        _scrollbar->update(content_width, content().width(), scroll().y());
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
        return {0, _host->size().y()};
    }
};

static inline RefPtr<HScroll> hscroll(RefPtr<Element> content = nullptr) { return make<HScroll>(content); }

} // namespace Widget
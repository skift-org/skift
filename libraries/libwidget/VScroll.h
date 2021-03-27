#pragma once

#include <libwidget/Component.h>
#include <libwidget/Container.h>
#include <libwidget/ScrollBar.h>

namespace Widget
{

class VScroll : public Component
{
private:
    Container *_host = nullptr;
    ScrollBar *_scrollbar = nullptr;

public:
    Container *host() { return _host; }

    VScroll(Component *parent) : Component(parent)
    {
        _host = new Container(this);

        _scrollbar = new ScrollBar(this);
        _scrollbar->flags(Component::NOT_AFFECTED_BY_SCROLL);

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

    Vec2i size() override
    {
        return {_host->size().x(), 0};
    }
};

} // namespace Widget

#pragma once

#include <libwidget/Container.h>
#include <libwidget/ScrollBar.h>
#include <libwidget/Widget.h>

class HScroll : public Widget
{
private:
    Container *_host = nullptr;
    ScrollBar *_scrollbar = nullptr;

public:
    Container *host() { return _host; }

    HScroll(Widget *parent) : Widget(parent)
    {
        _host = new Container(this);

        _scrollbar = new ScrollBar(this);
        _scrollbar->flags(Widget::NOT_AFFECTED_BY_SCROLL);

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

    Vec2i size() override
    {
        return {0, _host->size().y()};
    }
};
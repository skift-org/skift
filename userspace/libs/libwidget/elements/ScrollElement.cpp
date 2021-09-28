#include <libwidget/elements/ScrollElement.h>

namespace Widget
{

ScrollElement::ScrollElement(RefPtr<Element> content, bool horizontal)
    : _horizontal{horizontal}
{
    if (content != nullptr)
    {
        _host = add(content);
    }
    else
    {
        _host = add<Element>();
    }

    _scrollbar = add<ScrollBarElement>();
    _scrollbar->flags(Element::NOT_AFFECTED_BY_SCROLL);

    _scrollbar->on(Event::VALUE_CHANGE, [this](auto) {
        scroll({
            _horizontal ? _scrollbar->value() : scroll().x(),
            _horizontal ? scroll().y() : _scrollbar->value(),
        });

        should_repaint();
    });
}

void ScrollElement::layout()
{
    if (_horizontal)
    {
        int content_size = _host->size().x();

        _host->container(bound().take_left(content_size).cutoff_botton(ScrollBarElement::SIZE));
        _scrollbar->container(bound().take_bottom(ScrollBarElement::SIZE));
        _scrollbar->update(content_size, bound().width(), scroll().y());
    }
    else
    {
        int content_size = _host->size().y();

        _host->container(bound().take_top(content_size).cutoff_right(ScrollBarElement::SIZE));
        _scrollbar->container(bound().take_right(ScrollBarElement::SIZE));
        _scrollbar->update(content_size, bound().height(), scroll().x());
    }
}

void ScrollElement::event(Event *event)
{
    if (event->type == Event::MOUSE_SCROLL)
    {
        event->accepted = true;
        _scrollbar->dispatch_event(event);
    }
}

Math::Vec2i ScrollElement::size()
{
    return {
        _horizontal ? 0 : _host->size().x(),
        _horizontal ? _host->size().y() : 0,
    };
}

} // namespace Widget

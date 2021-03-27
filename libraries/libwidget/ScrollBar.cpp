#include <libgraphic/Painter.h>
#include <libwidget/Event.h>
#include <libwidget/ScrollBar.h>

namespace Widget
{

ScrollBar::ScrollBar(Component *parent) : Component(parent)
{
    _track = 1024;
    _value = 0;
    _thumb = 512;
}

void ScrollBar::paint(Graphic::Painter &painter, const Recti &)
{
    if (_thumb >= _track)
    {
        return;
    }

    if (_mouse_over || _mouse_drag)
    {
        painter.fill_rectangle_rounded(track_bound(), 4, color(THEME_BORDER));
        painter.fill_rectangle_rounded(thumb_bound(), 4, color(THEME_BORDER));
    }
    else
    {
        painter.fill_rectangle_rounded(thumb_bound(), 4, color(THEME_BORDER));
    }
}

void ScrollBar::scroll_to(Vec2i position)
{
    if (_horizontal)
    {
        int new_value = position.x() - track_bound().x();
        value((new_value / (float)track_bound().width()) * _track);
    }
    else
    {
        int new_value = position.y() - track_bound().y();
        value((new_value / (float)track_bound().height()) * _track);
    }

    Event event_value_changed = {};
    event_value_changed.type = Event::VALUE_CHANGE;
    dispatch_event(&event_value_changed);

    should_repaint();
}

void ScrollBar::event(Event *event)
{
    if (_thumb >= _track)
    {
        if (_mouse_over)
        {
            _mouse_over = false;
            should_repaint();
        }

        return;
    }

    if (is_mouse_event(event))
    {
        MouseEvent mouse_event = event->mouse;

        if (event->type == Event::MOUSE_ENTER)
        {
            _mouse_over = true;
            should_repaint();

            event->accepted = true;
        }
        else if (event->type == Event::MOUSE_LEAVE)
        {
            _mouse_over = false;

            should_repaint();

            event->accepted = true;
        }
        else if (event->type == Event::MOUSE_SCROLL)
        {
            value(value() + 48 * event->mouse.scroll);

            Event event_value_changed = {};
            event_value_changed.type = Event::VALUE_CHANGE;
            dispatch_event(&event_value_changed);

            should_repaint();

            event->accepted = true;
        }
        else if (event->type == Event::MOUSE_MOVE && mouse_event.buttons & MOUSE_BUTTON_LEFT)
        {
            Vec2i position = mouse_event.position - _mouse_origin;
            scroll_to(position);

            event->accepted = true;
        }
        else if (event->type == Event::MOUSE_BUTTON_PRESS &&
                 event->mouse.button == MOUSE_BUTTON_LEFT)
        {
            if (thumb_bound().contains(mouse_event.position))
            {
                _mouse_origin = mouse_event.position - thumb_bound().position();
            }
            else
            {
                scroll_to(mouse_event.position);
                _mouse_origin = thumb_bound().size() / 2;
            }

            _mouse_drag = true;

            event->accepted = true;
        }
        else if (event->type == Event::MOUSE_BUTTON_RELEASE &&
                 event->mouse.button == MOUSE_BUTTON_LEFT)
        {
            _mouse_drag = false;
            should_repaint();
        }
    }
}

} // namespace Widget

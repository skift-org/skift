#include <libgraphic/Painter.h>
#include <libwidget/Event.h>
#include <libwidget/widgets/ScrollBar.h>

ScrollBar::ScrollBar(Widget *parent) : Widget(parent)
{
    _track = 1024;
    _value = 0;
    _thumb = 512;
}

void ScrollBar::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (_thumb >= _track)
        return;

    if (_mouse_over)
    {
        painter.fill_rounded_rectangle(track_bound(), 4, color(THEME_BORDER));
        painter.fill_rounded_rectangle(thumb_bound(), 4, color(THEME_BORDER));
    }
    else
    {
        painter.fill_rounded_rectangle(thumb_bound(), 4, color(THEME_BORDER));
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

        if (event->type == Event::MOUSE_MOVE && mouse_event.buttons & MOUSE_BUTTON_LEFT)
        {
            Vec2i position = mouse_event.position - _mouse_origine;
            scroll_to(position);

            event->accepted = true;
        }
        else if (event->type == Event::MOUSE_BUTTON_PRESS)
        {
            if (thumb_bound().contains(mouse_event.position))
            {
                _mouse_origine = mouse_event.position - thumb_bound().position();
            }
            else
            {
                scroll_to(mouse_event.position);
                _mouse_origine = thumb_bound().size() / 2;
            }

            event->accepted = true;
        }
    }
}

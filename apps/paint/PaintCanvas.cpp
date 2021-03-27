#include <libgraphic/Painter.h>

#include "paint/PaintCanvas.h"
#include "paint/PaintTool.h"

PaintCanvas::PaintCanvas(Component *parent, RefPtr<PaintDocument> document)
    : Component(parent), _document(document)
{
}

void PaintCanvas::paint(Graphic::Painter &painter, const Recti &)
{
    auto paint_area = _document->bound().centered_within(bound());

    painter.fill_checkboard(paint_area, 8, Graphic::Colors::WHITE, Graphic::Colors::GAINSBORO);
    painter.draw_rectangle(paint_area, color(Widget::THEME_BORDER));

    painter.blit(_document->bitmap(), _document->bound(), paint_area);
    painter.draw_rectangle(paint_area, Graphic::Colors::WHITE.with_alpha(0.25));
}

void PaintCanvas::event(Widget::Event *event)
{
    if (is_mouse_event(event))
    {
        Widget::Event event_copy = *event;
        event_copy.mouse.old_position = event_copy.mouse.old_position - paint_area().position();
        event_copy.mouse.position = event_copy.mouse.position - paint_area().position();

        Graphic::Color color = Graphic::Colors::MAGENTA;

        if (event_copy.mouse.buttons & MOUSE_BUTTON_LEFT)
        {
            color = _document->primary_color();
        }
        else if (event_copy.mouse.buttons & MOUSE_BUTTON_RIGHT)
        {
            color = _document->secondary_color();
        }

        if (_tool)
        {
            _tool->event(*_document, event_copy, color);
        }

        if (event_copy.accepted)
        {
            event->accepted = true;
        }

        if (_document->dirty())
        {
            should_repaint();
            _document->dirty(false);
        }
    }
}

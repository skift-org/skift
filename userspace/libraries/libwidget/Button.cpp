#include <libgraphic/Painter.h>
#include <libwidget/Button.h>
#include <libwidget/IconPanel.h>
#include <libwidget/Image.h>
#include <libwidget/Label.h>

namespace Widget
{

void Button::paint(Graphic::Painter &painter, const Math::Recti &rectangle)
{
    UNUSED(rectangle);

    if (enabled())
    {
        if (_style == OUTLINE)
        {
            painter.draw_rectangle_rounded(bound(), 4, 1, color(THEME_BORDER));
        }
        else if (_style == FILLED)
        {
            painter.fill_rectangle_rounded(bound(), 4, color(THEME_ACCENT));
        }

        if (_mouse_over)
        {
            painter.fill_rectangle_rounded(bound(), 4, color(THEME_FOREGROUND).with_alpha(0.1));
        }

        if (_mouse_press)
        {
            painter.fill_rectangle_rounded(bound(), 4, color(THEME_FOREGROUND).with_alpha(0.1));
        }
    }
}

void Button::event(Event *event)
{
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
    else if (event->type == Event::MOUSE_BUTTON_PRESS)
    {
        _mouse_press = true;

        should_repaint();
        event->accepted = true;
    }
    else if (event->type == Event::MOUSE_BUTTON_RELEASE)
    {
        _mouse_press = false;

        Event action_event = {};
        action_event.type = Event::ACTION;
        dispatch_event(&action_event);

        should_repaint();
        event->accepted = true;
    }
    else if (event->type == Event::WIDGET_DISABLE)
    {
        _mouse_over = false;
        _mouse_press = false;
    }
}

Button::Button(Style style) : _style{style}
{
    layout(HFLOW(0));
    //insets(Insetsi(0, 16));
    min_height(36);
    flags(Element::GREEDY);
}

Button::Button(Style style, RefPtr<Graphic::Icon> icon) : Button{style}
{
    layout(STACK());

    min_width(64);
    min_height(36);
    flags(Element::GREEDY | Element::SQUARE);

    auto icon_panel = add<IconPanel>(icon);

    if (style == FILLED)
    {
        icon_panel->color(THEME_FOREGROUND, Graphic::Colors::WHITE);
    }
}

Button::Button(Style style, String text) : Button{style}
{
    layout(STACK());
    insets(Insetsi(0, 16));
    min_width(64);

    auto label = add<Label>(text, Anchor::CENTER);
    if (style == FILLED)
    {
        label->color(THEME_FOREGROUND, Graphic::Colors::WHITE);
    }
}

Button::Button(Style style, RefPtr<Graphic::Icon> icon, String text) : Button{style}
{
    insets(Insetsi(0, 0, 12, 16));
    min_width(64);

    auto icon_panel = add<IconPanel>(icon);
    icon_panel->insets(Insetsi(0, 0, 0, 8));

    auto label = add<Label>(text);

    if (style == FILLED)
    {
        label->color(THEME_FOREGROUND, Graphic::Colors::WHITE);
        icon_panel->color(THEME_FOREGROUND, Graphic::Colors::WHITE);
    }
}

Button::Button(Style style, RefPtr<Graphic::Bitmap> image, String text) : Button{style}
{
    insets(Insetsi(4, 4, 12, 16));
    min_width(64);

    auto image_panel = add<Image>(image, Graphic::BitmapScaling::FIT);
    image_panel->outsets(Insetsi(0, 0, 0, 8));
    image_panel->min_width(36);
    image_panel->min_height(36);

    auto label = add<Label>(text);

    if (style == FILLED)
    {
        label->color(THEME_FOREGROUND, Graphic::Colors::WHITE);
    }
}

} // namespace Widget

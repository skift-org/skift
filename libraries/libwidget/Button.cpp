#include <libgraphic/Painter.h>

#include <libwidget/Button.h>
#include <libwidget/IconPanel.h>
#include <libwidget/Image.h>
#include <libwidget/Label.h>

void Button::paint(Painter &painter, const Recti &rectangle)
{
    __unused(rectangle);

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

Button::Button(Widget *parent, Style style)
    : Widget(parent),
      _style(style)
{
    layout(HFLOW(0));
    insets(Insetsi(0, 6));
    min_height(32);
    flags(Widget::GREEDY);
}

Button::Button(Widget *parent, Style style, RefPtr<Icon> icon)
    : Button(parent, style)
{
    layout(STACK());
    insets(Insetsi(6));
    min_width(32);
    flags(Widget::GREEDY | Widget::SQUARE);

    auto icon_panel = new IconPanel(this, icon);

    if (style == FILLED)
    {
        icon_panel->color(THEME_FOREGROUND, Colors::WHITE);
    }
}

Button::Button(Widget *parent, Style style, String text)
    : Button(parent, style)
{
    layout(STACK());
    insets(Insetsi(0, 6));
    min_width(64);

    auto label = new Label(this, text, Anchor::CENTER);
    if (style == FILLED)
    {
        label->color(THEME_FOREGROUND, Colors::WHITE);
    }
}

Button::Button(Widget *parent, Style style, RefPtr<Icon> icon, String text)
    : Button(parent, style)
{
    insets(Insetsi(0, 0, 6, 10));
    min_width(64);

    auto icon_panel = new IconPanel(this, icon);
    icon_panel->insets(Insetsi(0, 0, 0, 4));

    auto label = new Label(this, text);

    if (style == FILLED)
    {
        label->color(THEME_FOREGROUND, Colors::WHITE);
        icon_panel->color(THEME_FOREGROUND, Colors::WHITE);
    }
}

Button::Button(Widget *parent, Style style, RefPtr<Bitmap> image, String text)
    : Button(parent, style)
{
    insets(Insetsi(4, 4, 6, 10));
    min_width(64);

    auto image_panel = new Image(this, image, BitmapScaling::FIT);
    image_panel->outsets(Insetsi(0, 0, 0, 8));
    image_panel->min_width(36);
    image_panel->min_height(36);

    auto label = new Label(this, text);

    if (style == FILLED)
    {
        label->color(THEME_FOREGROUND, Colors::WHITE);
    }
}

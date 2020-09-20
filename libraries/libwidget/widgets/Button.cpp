#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>

#include <libwidget/Widgets.h>
#include <libwidget/Window.h>

void Button::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (enabled())
    {
        if (_style == BUTTON_OUTLINE)
        {
            painter.draw_rounded_rectangle(bound(), 4, 1, color(THEME_BORDER));
        }
        else if (_style == BUTTON_FILLED)
        {
            painter.fill_rounded_rectangle(bound(), 4, color(THEME_ACCENT));
        }

        if (_state == BUTTON_OVER)
        {
            painter.fill_rounded_rectangle(bound(), 4, color(THEME_FOREGROUND).with_alpha(0.1));
        }

        if (_state == BUTTON_PRESS)
        {
            painter.fill_rounded_rectangle(bound(), 4, color(THEME_FOREGROUND).with_alpha(0.2));
        }
    }
}

void Button::event(Event *event)
{
    if (event->type == Event::MOUSE_ENTER)
    {
        _state = BUTTON_OVER;
        should_repaint();

        event->accepted = true;
    }
    else if (event->type == Event::MOUSE_LEAVE)
    {
        _state = BUTTON_IDLE;
        should_repaint();

        event->accepted = true;
    }
    else if (event->type == Event::MOUSE_BUTTON_PRESS)
    {
        _state = BUTTON_PRESS;
        should_repaint();

        event->accepted = true;
    }
    else if (event->type == Event::MOUSE_BUTTON_RELEASE)
    {
        _state = BUTTON_OVER;
        should_repaint();

        event->accepted = true;

        Event action_event = {};
        action_event.type = Event::ACTION;
        dispatch_event(&action_event);
    }
}

Button::Button(Widget *parent, ButtonStyle style)
    : Widget(parent),
      _style(style)
{
    layout(HFLOW(0));
    insets(Insets(0, 16));
    min_height(32);
    attributes(LAYOUT_GREEDY);
}

Button::Button(Widget *parent, ButtonStyle style, RefPtr<Icon> icon)
    : Button(parent, style)
{
    layout(STACK());
    insets(Insets(6));

    new IconPanel(this, icon);
}

Button::Button(Widget *parent, ButtonStyle style, String text)
    : Button(parent, style)
{
    layout(STACK());
    insets(Insets(0, 0, 6, 6));
    min_width(64);

    new Label(this, text, Position::CENTER);
}

Button::Button(Widget *parent, ButtonStyle style, RefPtr<Icon> icon, String text)
    : Button(parent, style)
{
    insets(Insets(0, 0, 6, 10));
    min_width(64);

    auto icon_panel = new IconPanel(this, icon);
    icon_panel->insets(Insets(0, 0, 0, 4));

    new Label(this, text);
}

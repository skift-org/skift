#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Element.h>

namespace Widget
{

class Button : public Element
{
public:
    enum Style
    {
        TEXT,
        OUTLINE,
        FILLED,
    };

private:
    bool _mouse_over = false;
    bool _mouse_press = false;
    Style _style = TEXT;

public:
    Button(Style style);

    Button(Style style, RefPtr<Graphic::Icon> icon);

    Button(Style style, String text);

    Button(Style style, RefPtr<Graphic::Icon> icon, String text);

    Button(Style style, RefPtr<Graphic::Bitmap> image, String text);

    void paint(Graphic::Painter &painter, const Math::Recti &rectangle) override;

    void event(Event *event) override;
};

static inline RefPtr<Button> button(Button::Style style, Callback<void(void)> on_click = nullptr)
{
    auto button = make<Button>(style);
    if (on_click)
    {
        button->on(Event::ACTION, [on_click](auto) { on_click(); });
    }
    return button;
}

static inline RefPtr<Button> button(Button::Style style, RefPtr<Graphic::Icon> icon, Callback<void(void)> on_click = nullptr)
{
    auto button = make<Button>(style, icon);
    if (on_click)
    {
        button->on(Event::ACTION, [on_click](auto) { on_click(); });
    }
    return button;
}

static inline RefPtr<Button> button(Button::Style style, String text, Callback<void(void)> on_click = nullptr)
{
    auto button = make<Button>(style, text);
    if (on_click)
    {
        button->on(Event::ACTION, [on_click](auto) { on_click(); });
    }
    return button;
}

static inline RefPtr<Button> button(Button::Style style, RefPtr<Graphic::Icon> icon, String text, Callback<void(void)> on_click = nullptr)
{
    auto button = make<Button>(style, icon, text);
    if (on_click)
    {
        button->on(Event::ACTION, [on_click](auto) { on_click(); });
    }
    return button;
}

static inline RefPtr<Button> button(Button::Style style, RefPtr<Graphic::Bitmap> image, String text, Callback<void(void)> on_click = nullptr)
{
    auto button = make<Button>(style, image, text);
    if (on_click)
    {
        button->on(Event::ACTION, [on_click](auto) { on_click(); });
    }
    return button;
}

} // namespace Widget

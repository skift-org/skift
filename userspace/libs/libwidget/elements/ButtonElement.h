#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Element.h>

namespace Widget
{

struct ButtonElement : public Element
{
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
    ButtonElement(Style style);

    void paint(Graphic::Painter &painter, const Math::Recti &rectangle) override;

    void event(Event *event) override;
};

Ref<ButtonElement> button(ButtonElement::Style style, Ref<Element> child, Func<void(void)> on_click);

/* --- Basic Button --------------------------------------------------------- */

Ref<ButtonElement> basic_button(Ref<Element> child, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> basic_button(Ref<Graphic::Icon> icon, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> basic_button(String text, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> basic_button(Ref<Graphic::Icon> icon, String text, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> basic_button(Ref<Graphic::Bitmap> image, String text, Func<void(void)> on_click = nullptr);

/* --- Outlined Button ------------------------------------------------------ */

Ref<ButtonElement> outline_button(Ref<Element> child, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> outline_button(Ref<Graphic::Icon> icon, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> outline_button(String text, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> outline_button(Ref<Graphic::Icon> icon, String text, Func<void(void)> on_click = nullptr);

/* --- Filled Button -------------------------------------------------------- */

Ref<ButtonElement> filled_button(Ref<Element> child, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> filled_button(Ref<Graphic::Icon> icon, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> filled_button(String text, Func<void(void)> on_click = nullptr);

Ref<ButtonElement> filled_button(Ref<Graphic::Icon> icon, String text, Func<void(void)> on_click = nullptr);

} // namespace Widget

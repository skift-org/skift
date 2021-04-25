#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Component.h>

namespace Widget
{

class Button : public Component
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

} // namespace Widget

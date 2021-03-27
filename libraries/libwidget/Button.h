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
    Button(Component *parent, Style style);

    Button(Component *parent, Style style, RefPtr<Graphic::Icon> icon);

    Button(Component *parent, Style style, String text);

    Button(Component *parent, Style style, RefPtr<Graphic::Icon> icon, String text);

    Button(Component *parent, Style style, RefPtr<Graphic::Bitmap> image, String text);

    void paint(Graphic::Painter &painter, const Recti &rectangle) override;

    void event(Event *event) override;
};

} // namespace Widget

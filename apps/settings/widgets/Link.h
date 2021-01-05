#pragma once

#include <libwidget/Widgets.h>

namespace settings
{

class Link : public Button
{
private:
public:
    Link(Widget *parent, RefPtr<Icon> icon, String name)
        : Button(parent, BUTTON_TEXT)
    {
        layout(VFLOW(4));
        insets({8, 24});

        auto icon_container = new IconPanel(this, icon);
        icon_container->icon_size(ICON_36PX);
        icon_container->flags(Widget::FILL);
        new Label(this, name, Anchor::CENTER);
    }

    ~Link() {}
};

} // namespace settings

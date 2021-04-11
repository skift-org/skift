#include <libwidget/IconPanel.h>
#include <libwidget/Label.h>

#include "settings/widgets/Link.h"

namespace Settings
{

Link::Link(Component *parent, RefPtr<Graphic::Icon> icon, String name)
    : Button(parent, Button::TEXT)
{
    layout(VFLOW(4));
    insets({8, 24});

    auto icon_container = new Widget::IconPanel(this, icon);
    icon_container->icon_size(Graphic::ICON_36PX);
    icon_container->flags(Component::FILL);
    new Widget::Label(this, name, Anchor::CENTER);
}

} // namespace Settings

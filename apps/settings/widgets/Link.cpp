#include <libwidget/IconPanel.h>
#include <libwidget/Label.h>

#include "settings/widgets/Link.h"

namespace Settings
{

Link::Link(Widget *parent, RefPtr<Icon> icon, String name)
    : Button(parent, Button::TEXT)
{
    layout(VFLOW(4));
    insets({8, 24});

    auto icon_container = new IconPanel(this, icon);
    icon_container->icon_size(ICON_36PX);
    icon_container->flags(Widget::FILL);
    new Label(this, name, Anchor::CENTER);
}

} // namespace Settings

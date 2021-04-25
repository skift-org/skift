#include <libwidget/IconPanel.h>
#include <libwidget/Label.h>

#include "settings/widgets/Link.h"

namespace Settings
{

Link::Link(RefPtr<Graphic::Icon> icon, String name)
    : Button(Button::TEXT)
{
    layout(VFLOW(4));
    insets({8, 24});

    auto icon_container = add<Widget::IconPanel>(icon);
    icon_container->icon_size(Graphic::ICON_36PX);
    icon_container->flags(Element::FILL);
    add<Widget::Label>(name, Anchor::CENTER);
}

} // namespace Settings

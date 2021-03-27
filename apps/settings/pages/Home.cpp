#include "settings/pages/Home.h"
#include "settings/widgets/Link.h"

namespace Settings
{

HomePage::HomePage(Component *parent)
    : Container(parent)
{
    layout(STACK());
    flags(Component::FILL);

    auto links = new Container(this);
    links->layout(GRID(6, 4, 8, 8));
    links->insets(16);

    new Link(links, Graphic::Icon::get("home"), "test");
    new Link(links, Graphic::Icon::get("home"), "test");
    new Link(links, Graphic::Icon::get("home"), "test");
    new Link(links, Graphic::Icon::get("home"), "test");
    new Link(links, Graphic::Icon::get("home"), "test");
    new Link(links, Graphic::Icon::get("home"), "test");
    new Link(links, Graphic::Icon::get("home"), "test");
    new Link(links, Graphic::Icon::get("home"), "test");
    new Link(links, Graphic::Icon::get("home"), "test");
}

} // namespace Settings

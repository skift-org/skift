#include "settings/pages/Home.h"
#include "settings/widgets/Link.h"

namespace Settings
{

HomePage::HomePage(Widget *parent)
    : Container(parent)
{
    layout(STACK());
    flags(Widget::FILL);

    auto links = new Container(this);
    links->layout(GRID(6, 4, 8, 8));
    links->insets(16);

    new Link(links, Icon::get("home"), "test");
    new Link(links, Icon::get("home"), "test");
    new Link(links, Icon::get("home"), "test");
    new Link(links, Icon::get("home"), "test");
    new Link(links, Icon::get("home"), "test");
    new Link(links, Icon::get("home"), "test");
    new Link(links, Icon::get("home"), "test");
    new Link(links, Icon::get("home"), "test");
    new Link(links, Icon::get("home"), "test");
}

} // namespace Settings

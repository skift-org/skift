#include "settings/pages/Home.h"
#include "settings/widgets/Link.h"

namespace Settings
{

HomePage::HomePage()
    : Container()
{
    layout(STACK());
    flags(Component::FILL);

    auto links = add<Container>();
    links->layout(GRID(6, 4, 8, 8));
    links->insets(16);

    links->add<Link>(Graphic::Icon::get("home"), "test");
    links->add<Link>(Graphic::Icon::get("home"), "test");
    links->add<Link>(Graphic::Icon::get("home"), "test");
    links->add<Link>(Graphic::Icon::get("home"), "test");
    links->add<Link>(Graphic::Icon::get("home"), "test");
    links->add<Link>(Graphic::Icon::get("home"), "test");
    links->add<Link>(Graphic::Icon::get("home"), "test");
    links->add<Link>(Graphic::Icon::get("home"), "test");
    links->add<Link>(Graphic::Icon::get("home"), "test");
}

} // namespace Settings

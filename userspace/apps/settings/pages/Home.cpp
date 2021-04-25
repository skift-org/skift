#include "settings/pages/Home.h"
#include "settings/widgets/Link.h"

namespace Settings
{

HomePage::HomePage()
{
    layout(STACK());
    flags(Element::FILL);
}

RefPtr<Widget::Element> HomePage::build()
{
    using namespace Widget;

    // clang-format off
    return stack({
        spacing(16,
            grid(4, 4, 8, 8,
                {
                    link(icon("home",Graphic::ICON_36PX), "test"),
                    link(icon("home",Graphic::ICON_36PX), "test"),
                    link(icon("home",Graphic::ICON_36PX), "test"),
                    link(icon("home",Graphic::ICON_36PX), "test"),
                    link(icon("home",Graphic::ICON_36PX), "test"),
                    link(icon("home",Graphic::ICON_36PX), "test"),
                    link(icon("home",Graphic::ICON_36PX), "test"),
                    link(icon("home",Graphic::ICON_36PX), "test"),
                    link(icon("home",Graphic::ICON_36PX), "test"),
                }
            )
        )
    });
    // clang-format on
}

} // namespace Settings

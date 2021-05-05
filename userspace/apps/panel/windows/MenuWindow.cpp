#include <libwidget/Screen.h>

#include "libwidget/components/StatefulComponent.h"
#include "libwidget/layouts/Flags.h"
#include "libwidget/layouts/FlowLayout.h"
#include "panel/model/MenuEntry.h"
#include "panel/widgets/ApplicationListing.h"
#include "panel/widgets/SearchBar.h"
#include "panel/windows/MenuWindow.h"
#include "panel/windows/PanelWindow.h"

namespace panel
{

MenuWindow::MenuWindow()
    : Window(WINDOW_ALWAYS_FOCUSED | WINDOW_AUTO_CLOSE | WINDOW_ACRYLIC)
{
    bound((Math::Recti{4, PanelWindow::HEIGHT, 500, 400}));
    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);

    on(Widget::Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Widget::Screen::bound().with_width(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}));
    });

    on(Widget::Event::KEYBOARD_KEY_PRESS, [this](Widget::Event *event) {
        if (event->keyboard.key == KEYBOARD_KEY_ESC)
        {
            hide();
            event->accepted = true;
        }
    });

    on(Widget::Event::GOT_FOCUS, [&](auto) {
        should_rebuild();
    });
}

RefPtr<Widget::Element> MenuWindow::build()
{
    // clang-format off

    return Widget::spacing(6,  
        Widget::stateful<String>([](auto state, auto update){
            return Widget::vflow(
                4,
                {
                    search_bar(state, [=](auto text){update(text);}),
                    application_listing(state),
                }
            );
        })
    );

    // clang-format on
}

} // namespace panel
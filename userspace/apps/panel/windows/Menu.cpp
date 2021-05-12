#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Screen.h>

#include "panel/Constants.h"
#include "panel/widgets/ApplicationListing.h"
#include "panel/widgets/SearchBar.h"
#include "panel/windows/Menu.h"

using namespace Widget;

namespace Panel
{

Menu::Menu() : Window(WINDOW_ALWAYS_FOCUSED | WINDOW_AUTO_CLOSE | WINDOW_ACRYLIC)
{
    bound(Math::Recti{
        4,
        PANEL_WINDOW_HEIGHT,
        MENU_WINDOW_WIDTH,
        MENU_WINDOW_HEIGHT,
    });

    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);

    on(Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Screen::bound()
                  .with_width(MENU_WINDOW_WIDTH)
                  .shrinked({PANEL_WINDOW_HEIGHT, 0, 0, 0}));
    });

    on(Event::KEYBOARD_KEY_PRESS, [this](Event *event) {
        if (event->keyboard.key == KEYBOARD_KEY_ESC)
        {
            hide();
            event->accepted = true;
        }
    });

    on(Event::GOT_FOCUS, [&](auto) {
        should_rebuild();
    });
}

RefPtr<Element> Menu::build()
{
    // clang-format off

    return spacing(6,
        stateful<String>([](auto state, auto update){
            return vflow(
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

} // namespace Panel

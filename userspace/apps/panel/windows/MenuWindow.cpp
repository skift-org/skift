#include <libwidget/Container.h>
#include <libwidget/Screen.h>

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

    root()->layout(HFLOW(0));
    root()->insets(6);

    auto container = root()->add<Widget::Container>();
    container->layout(VFLOW(0));
    container->flags(Widget::Element::FILL);

    auto model = Widget::TextModel::empty();

    container->add<SearchBar>(model);

    auto listing = container->add<ApplicationListing>();

    _search_query_observer = model->observe([listing](auto &text) {
        listing->filter(text.string());
    });

    on(Widget::Event::KEYBOARD_KEY_PRESS, [this](Widget::Event *event) {
        if (event->keyboard.key == KEYBOARD_KEY_ESC)
        {
            hide();
            event->accepted = true;
        }
    });

    on(Widget::Event::GOT_FOCUS, [model](auto) {
        model->clear();
    });
}

} // namespace panel
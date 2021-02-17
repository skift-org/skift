#include <libsystem/process/Process.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Panel.h>
#include <libwidget/Screen.h>
#include <libwidget/Separator.h>
#include <libwidget/Spacer.h>
#include <skift/Environment.h>

#include "panel/model/MenuEntry.h"
#include "panel/widgets/ApplicationListing.h"
#include "panel/widgets/SearchBar.h"
#include "panel/windows/MenuWindow.h"
#include "panel/windows/PanelWindow.h"

namespace panel
{

MenuWindow::MenuWindow()
    : Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED | WINDOW_AUTO_CLOSE | WINDOW_ACRYLIC | WINDOW_NO_ROUNDED_CORNERS)
{
    title("Panel");
    bound(Screen::bound().with_width(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}));
    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);

    on(Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Screen::bound().with_width(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}));
    });

    root()->layout(HFLOW(0));

    auto container = new Container(root());
    container->layout(VFLOW(0));
    container->flags(Widget::FILL);

    new Separator(root());

    auto model = TextModel::empty();

    new SearchBar(container, model);

    auto listing = new ApplicationListing(container);

    _search_query_observer = model->observe([listing](auto &text) {
        listing->filter(text.string());
    });

    auto account_container = new Panel(container);
    account_container->layout(HFLOW(4));
    account_container->insets({6});

    new Button(account_container, Button::TEXT, Icon::get("account"), environment().get("POSIX").get("LOGNAME").as_string());

    new Spacer(account_container);

    auto folder_button = new Button(account_container, Button::TEXT, Icon::get("folder"));

    folder_button->on(EventType::ACTION, [&](auto) {
        process_run("file-manager", nullptr);
    });

    auto setting_button = new Button(account_container, Button::TEXT, Icon::get("cog"));

    setting_button->on(EventType::ACTION, [&](auto) {
        process_run("settings", nullptr);
    });

    auto logout_button = new Button(account_container, Button::TEXT, Icon::get("power-standby"));
    logout_button->on(EventType::ACTION, [&](auto) {
        process_run("logout", nullptr);
    });

    on(Event::KEYBOARD_KEY_PRESS, [this](Event *event) {
        if (event->keyboard.key == KEYBOARD_KEY_ESC)
        {
            hide();
            event->accepted = true;
        }
    });

    on(Event::GOT_FOCUS, [model](auto) {
        model->clear();
    });
}

} // namespace panel
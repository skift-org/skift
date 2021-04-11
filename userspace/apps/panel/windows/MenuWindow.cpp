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
    bound(Widget::Screen::bound().with_width(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}));
    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);

    on(Widget::Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Widget::Screen::bound().with_width(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}));
    });

    root()->layout(HFLOW(0));

    auto container = new Widget::Container(root());
    container->layout(VFLOW(0));
    container->flags(Widget::Component::FILL);

    new Widget::Separator(root());

    auto model = Widget::TextModel::empty();

    new SearchBar(container, model);

    auto listing = new ApplicationListing(container);

    _search_query_observer = model->observe([listing](auto &text) {
        listing->filter(text.string());
    });

    auto account_container = new Widget::Panel(container);
    account_container->layout(HFLOW(4));
    account_container->insets({6});

    new Widget::Button(account_container, Widget::Button::TEXT, Graphic::Icon::get("account"), environment().get("POSIX").get("LOGNAME").as_string());

    new Widget::Spacer(account_container);

    auto folder_button = new Widget::Button(account_container, Widget::Button::TEXT, Graphic::Icon::get("folder"));

    folder_button->on(Widget::EventType::ACTION, [&](auto) {
        process_run("file-manager", nullptr, 0);
    });

    auto setting_button = new Widget::Button(account_container, Widget::Button::TEXT, Graphic::Icon::get("cog"));

    setting_button->on(Widget::EventType::ACTION, [&](auto) {
        process_run("settings", nullptr, 0);
    });

    auto logout_button = new Widget::Button(account_container, Widget::Button::TEXT, Graphic::Icon::get("power-standby"));
    logout_button->on(Widget::EventType::ACTION, [&](auto) {
        process_run("logout", nullptr, 0);
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
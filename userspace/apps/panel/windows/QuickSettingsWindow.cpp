#include <skift/Environment.h>

#include <libwidget/Container.h>
#include <libwidget/Elements.h>
#include <libwidget/Screen.h>

#include "panel/widgets/SettingToggle.h"
#include "panel/windows/PanelWindow.h"
#include "panel/windows/QuickSettingsWindow.h"

namespace panel
{

QuickSettingsWindow::QuickSettingsWindow()
    : Window(WINDOW_AUTO_CLOSE | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC)
{
    bound(Widget::Screen::bound().take_right(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}).with_height(HEIGHT));
    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);

    on(Widget::Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Widget::Screen::bound().take_right(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}).with_height(HEIGHT));
    });

    root()->layout(VFLOW(4));
    root()->insets(6);

    auto title = root()->add<Widget::Label>("Quick settings");
    title->outsets({12, 6, 12, 12});

    root()->add<SettingToggle>("Show Wireframe", Graphic::Icon::get("duck"), "appearance:widgets.wireframe");
    root()->add<SettingToggle>("Night Light", Graphic::Icon::get("moon-waning-crescent"), "appearance:night-light.enable");

    root()->add<Widget::Button>(Widget::Button::TEXT, "Light Theme")->on(Widget::Event::ACTION, [](auto) {
        Settings::Service::the()->write(Settings::Path::parse("appearance:widgets.theme"), "skift-light");
    });

    root()->add<Widget::Button>(Widget::Button::TEXT, "Dark Theme")->on(Widget::Event::ACTION, [](auto) {
        Settings::Service::the()->write(Settings::Path::parse("appearance:widgets.theme"), "skift-dark");
    });

    auto account_container = root()->add<Widget::Container>();
    account_container->layout(HFLOW(4));

    account_container->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("account"), environment().get("POSIX").get("LOGNAME").as_string());

    account_container->add(Widget::spacer());

    auto folder_button = account_container->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("folder"));

    folder_button->on(Widget::EventType::ACTION, [&](auto) {
        process_run("file-manager", nullptr, 0);
    });

    auto setting_button = account_container->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("cog"));

    setting_button->on(Widget::EventType::ACTION, [&](auto) {
        process_run("settings", nullptr, 0);
    });

    auto logout_button = account_container->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("power-standby"));
    logout_button->on(Widget::EventType::ACTION, [&](auto) {
        process_run("logout", nullptr, 0);
    });

    bound(bound_on_screen().with_height(root()->compute_size().y()));
}

} // namespace panel
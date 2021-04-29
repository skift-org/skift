#include <skift/Environment.h>

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

    auto title = root()->add(Widget::label("Quick settings"));
    title->outsets({12, 6, 12, 12});

    root()->add<SettingToggle>("Show Wireframe", Graphic::Icon::get("duck"), "appearance:widgets.wireframe");
    root()->add<SettingToggle>("Night Light", Graphic::Icon::get("moon-waning-crescent"), "appearance:night-light.enable");

    root()->add(Widget::basic_button("Light Theme", [] {
        Settings::Service::the()->write(Settings::Path::parse("appearance:widgets.theme"), "skift-light");
    }));

    root()->add(Widget::basic_button("Dark Theme", [] {
        Settings::Service::the()->write(Settings::Path::parse("appearance:widgets.theme"), "skift-dark");
    }));

    auto account_container = root()->add<Widget::Element>();
    account_container->layout(HFLOW(4));

    account_container->add(Widget::basic_button(Graphic::Icon::get("account"), environment().get("POSIX").get("LOGNAME").as_string()));

    account_container->add(Widget::spacer());

    auto folder_button = account_container->add(Widget::basic_button(Graphic::Icon::get("folder"), [&] {
        process_run("file-manager", nullptr, 0);
    }));

    auto setting_button = account_container->add(Widget::basic_button(Graphic::Icon::get("cog"), [&] {
        process_run("settings", nullptr, 0);
    }));

    auto logout_button = account_container->add(Widget::basic_button(Graphic::Icon::get("power-standby"), [&] {
        process_run("logout", nullptr, 0);
    }));

    bound(bound_on_screen().with_height(root()->compute_size().y()));
}

} // namespace panel
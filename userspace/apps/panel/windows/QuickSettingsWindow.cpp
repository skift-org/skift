#include <libwidget/Screen.h>

#include "panel/widgets/SettingToggle.h"
#include "panel/windows/PanelWindow.h"
#include "panel/windows/QuickSettingsWindow.h"

namespace panel
{

QuickSettingsWindow::QuickSettingsWindow()
    : Window(WINDOW_AUTO_CLOSE | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC)
{
    title("Panel");
    bound(Widget::Screen::bound().take_right(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}).with_height(HEIGHT));
    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);

    on(Widget::Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Widget::Screen::bound().take_right(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}).with_height(HEIGHT));
    });

    root()->layout(VFLOW(4));
    root()->insets(6);

    auto title = new Widget::Label(root(), "Quick settings");
    title->outsets({12, 6, 12, 12});

    new SettingToggle(root(), "Show Wireframe", Graphic::Icon::get("duck"), "appearance:widgets.wireframe");
    new SettingToggle(root(), "Night Light", Graphic::Icon::get("moon-waning-crescent"), "appearance:night-light.enable");

    (new Widget::Button(root(), Widget::Button::TEXT, "Light Theme"))->on(Widget::Event::ACTION, [](auto) {
        Settings::Service::the()->write(Settings::Path::parse("appearance:widgets.theme"), "skift-light");
    });

    (new Widget::Button(root(), Widget::Button::TEXT, "Dark Theme"))->on(Widget::Event::ACTION, [](auto) {
        Settings::Service::the()->write(Settings::Path::parse("appearance:widgets.theme"), "skift-dark");
    });

    bound(bound_on_screen().with_height(root()->compute_size().y()));
}

} // namespace panel
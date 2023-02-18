#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

Ui::Child sidebar() {
    return Ui::vscroll(
        Ui::spacing(
            {0, 8},

            Ui::vflow(
                8,

                Ui::buttonRow(NONE, Media::Icons::ACCOUNT, "Accounts", "User, Online Accounts, etc."),
                Ui::buttonRow(NONE, Media::Icons::PALETTE, "Personalization", "Wallpaper, Themes, etc."),
                Ui::buttonRow(NONE, Media::Icons::WIDGETS_OUTLINE, "Applications", "Installed Apps, etc."),

                Ui::buttonRow(NONE, Media::Icons::LAPTOP, "System", "Display, Keyboard, Mouse, etc."),
                Ui::buttonRow(NONE, Media::Icons::WIFI, "Network", "Wi-Fi, Bluetooth, etc."),
                Ui::buttonRow(NONE, Media::Icons::SECURITY, "Security & Privacy", "Firewall, Passwords, etc."),

                Ui::buttonRow(NONE, Media::Icons::UPDATE, "Updates", "Software, Drivers, etc."),
                Ui::buttonRow(NONE, Media::Icons::INFORMATION_OUTLINE, "About", "System Information, etc."))));
}

Ui::Child content() {
    return Ui::grow(
        Ui::center(
            Ui::text("Content")));
}

Res<> entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(
        Media::Icons::COG,
        "Settings",
        Ui::TitlebarStyle::DEFAULT);

    auto layout = Ui::dialogLayer(
        Ui::minSize(
            {700, 500},
            Ui::vflow(
                titlebar,
                Ui::toolbar(
                    Ui::button(NONE, Ui::ButtonStyle::subtle(), Media::Icons::MENU),
                    Ui::button(NONE, Ui::ButtonStyle::subtle(), Media::Icons::ARROW_LEFT),
                    Ui::button(NONE, Ui::ButtonStyle::subtle(), Media::Icons::ARROW_RIGHT)),
                Ui::grow(
                    Ui::hflow(
                        sidebar(),
                        Ui::separator(),
                        content())))));

    return Ui::runApp(
        args, layout);
}

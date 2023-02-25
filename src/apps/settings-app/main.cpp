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

                Ui::buttonRow(NONE, Mdi::ACCOUNT, "Accounts", "User, Online Accounts, etc."),
                Ui::buttonRow(NONE, Mdi::PALETTE, "Personalization", "Wallpaper, Themes, etc."),
                Ui::buttonRow(NONE, Mdi::WIDGETS_OUTLINE, "Applications", "Installed Apps, etc."),

                Ui::buttonRow(NONE, Mdi::LAPTOP, "System", "Display, Keyboard, Mouse, etc."),
                Ui::buttonRow(NONE, Mdi::WIFI, "Network", "Wi-Fi, Bluetooth, etc."),
                Ui::buttonRow(NONE, Mdi::SECURITY, "Security & Privacy", "Firewall, Passwords, etc."),

                Ui::buttonRow(NONE, Mdi::UPDATE, "Updates", "Software, Drivers, etc."),
                Ui::buttonRow(NONE, Mdi::INFORMATION_OUTLINE, "About", "System Information, etc."))));
}

Ui::Child content() {
    return Ui::grow(
        Ui::center(
            Ui::text("Content")));
}

Res<> entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(
        Mdi::COG,
        "Settings",
        Ui::TitlebarStyle::DEFAULT);

    auto layout = Ui::dialogLayer(
        Ui::minSize(
            {700, 500},
            Ui::vflow(
                titlebar,
                Ui::toolbar(
                    Ui::button(NONE, Ui::ButtonStyle::subtle(), Mdi::MENU),
                    Ui::button(NONE, Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
                    Ui::button(NONE, Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT)),
                Ui::grow(
                    Ui::hflow(
                        sidebar(),
                        Ui::separator(),
                        content())))));

    return Ui::runApp(
        args, layout);
}

#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>
#include <karm-ui/text.h>

Ui::Child settingRow(Media::Icons icon, String title, String subtitle) {
    return Ui::spacing(
        {8, 0},
        Ui::button(
            Ui::NOP,
            Ui::Button::SUBTLE,
            Ui::row(
                Ui::icon(
                    icon,
                    24),
                title,
                subtitle,
                NONE)));
}

Ui::Child sidebar() {
    return Ui::spacing(
        {0, 8},

        Ui::vflow(
            8,

            settingRow(Media::Icons::ACCOUNT, "Accounts", "User, Online Accounts, etc."),
            settingRow(Media::Icons::PALETTE, "Personalization", "Wallpaper, Themes, etc."),
            settingRow(Media::Icons::WIDGETS_OUTLINE, "Applications", "Installed Apps, etc."),

            settingRow(Media::Icons::LAPTOP, "System", "Display, Keyboard, Mouse, etc."),
            settingRow(Media::Icons::WIFI, "Network", "Wi-Fi, Bluetooth, etc."),
            settingRow(Media::Icons::SECURITY, "Security & Privacy", "Firewall, Passwords, etc."),

            settingRow(Media::Icons::UPDATE, "Updates", "Software, Drivers, etc."),
            settingRow(Media::Icons::INFORMATION_OUTLINE, "About", "System Information, etc.")));
}

Ui::Child content() {
    return Ui::grow(Ui::center(Ui::text("Content")));
}

CliResult
entryPoint(CliArgs args) {
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
                    Ui::button(Ui::NOP, Ui::Button::SUBTLE, Media::Icons::MENU),
                    Ui::button(Ui::NOP, Ui::Button::SUBTLE, Media::Icons::ARROW_LEFT),
                    Ui::button(Ui::NOP, Ui::Button::SUBTLE, Media::Icons::ARROW_RIGHT)),
                Ui::grow(Ui::hflow(sidebar(), Ui::separator(), content())))));

    return Ui::runApp(
        args, layout);
}

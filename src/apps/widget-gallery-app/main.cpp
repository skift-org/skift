#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/text.h>

void willShowMessage(Ui::Node &n) {
    Ui::showMsgDialog(n, "Button pressed");
}

Ui::Child buttons(bool enable) {
    return Ui::hflow(
        8,
        Ui::button(enable ? willShowMessage : Ui::OnPress{}, Ui::ButtonStyle::primary(), "PRIMARY BUTTON"),
        Ui::button(enable ? willShowMessage : Ui::OnPress{}, "BUTTON"),
        Ui::button(enable ? willShowMessage : Ui::OnPress{}, Ui::ButtonStyle::outline(), "OUTLINE BUTTON"),
        Ui::button(enable ? willShowMessage : Ui::OnPress{}, Ui::ButtonStyle::subtle(), "SUBTLE BUTTON"),
        Ui::button(enable ? willShowMessage : Ui::OnPress{}, Ui::ButtonStyle::destructive(), "DESTRUCTIVE BUTTON"));
}

Ui::Child badges() {
    return Ui::hflow(
        8,
        Ui::badge(Ui::BadgeStyle::ERROR, "Error"),
        Ui::badge(Ui::BadgeStyle::WARNING, "Warning"),
        Ui::badge(Ui::BadgeStyle::SUCCESS, "Success"),
        Ui::badge(Ui::BadgeStyle::INFO, "Info"));
}

Ui::Child texts() {
    return Ui::vflow(
        8,
        Ui::text(12, "The quick brown fox jumps over the lazy dog."),
        Ui::text(14, "The quick brown fox jumps over the lazy dog."),
        Ui::text(18, "The quick brown fox jumps over the lazy dog."),
        Ui::text(22, "The quick brown fox jumps over the lazy dog."),
        Ui::text(36, "The quick brown fox jumps over the lazy dog."),
        Ui::text(48, "The quick brown fox jumps over the lazy dog."));
}

CliResult entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(Media::Icons::DUCK, "Widget Gallery");
    auto content = Ui::spacing(
        8,
        Ui::vflow(
            8,
            buttons([](Ui::Node &n) {
                Ui::showMsgDialog(n, "Button pressed");
            }),
            buttons(NONE),
            badges(),
            texts()));

    auto layout = Ui::dialogLayer(
        Ui ::minSize(
            {700, 500},
            Ui::vflow(titlebar,
                      Ui::grow(
                          Ui::scroll(content)))));

    return Ui::runApp(args, layout);
}

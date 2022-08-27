#include <karm-main/main.h>
#include <karm-ui/align.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/flow.h>

void nop() {}

Ui::Child winControls() {

    static constexpr Ui::ButtonStyle SUBTLE = {
        .idleStyle = {
            .foregroundColor = Gfx::ZINC400,
        },
        .hoverStyle = {
            .borderRadius = Ui::Button::RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC500,
            .backgroundColor = Gfx::ZINC600,
        },
        .pressStyle = {
            .borderRadius = Ui::Button::RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::ZINC600,
            .backgroundColor = Gfx::ZINC700,
        },
    };

    static constexpr Ui::ButtonStyle CLOSE = {
        .idleStyle = {
            .borderRadius = Ui::Button::RADIUS,
            .foregroundColor = Gfx::ZINC400,
        },
        .hoverStyle = {
            .borderRadius = Ui::Button::RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::RED500,
            .backgroundColor = Gfx::RED600,
        },
        .pressStyle = {
            .borderRadius = Ui::Button::RADIUS,
            .borderWidth = 1,
            .borderColor = Gfx::RED600,
            .backgroundColor = Gfx::RED700,
        },
    };

    return Ui::hflow(
        4,
        Ui::button(nop, SUBTLE, Media::Icons::WINDOW_MINIMIZE),
        Ui::button(nop, SUBTLE, Media::Icons::WINDOW_MAXIMIZE),
        Ui::button(nop, CLOSE, Media::Icons::WINDOW_CLOSE));
}

Ui::Child buttons() {
    return Ui::vflow(
        8,
        winControls(),
        Ui::button(nop, Ui::Button::PRIMARY, "PRIMARY BUTTON"),
        Ui::button(nop, "BUTTON"),
        Ui::button(nop, Ui::Button::OUTLINE, "OUTLINE BUTTON"),
        Ui::button(nop, Ui::Button::SUBTLE, "SUBTLE BUTTON"),
        Ui::button(nop, Ui::Button::DESTRUCTIVE, "DESTRUCTIVE BUTTON"));
}

Ui::Child texts() {
    auto fontface = Media::loadFontface("res/fonts/inter/Inter-Regular.ttf").unwrap();
    return Ui::vflow(
        8,
        Ui::text(Media::Font{12, fontface}, "The quick brown fox jumps over the lazy dog."),
        Ui::text(Media::Font{14, fontface}, "The quick brown fox jumps over the lazy dog."),
        Ui::text(Media::Font{18, fontface}, "The quick brown fox jumps over the lazy dog."),
        Ui::text(Media::Font{22, fontface}, "The quick brown fox jumps over the lazy dog."),
        Ui::text(Media::Font{36, fontface}, "The quick brown fox jumps over the lazy dog."),
        Ui::text(Media::Font{48, fontface}, "The quick brown fox jumps over the lazy dog."));
}

CliResult entryPoint(CliArgs args) {
    Ui::App app([]() {
        return Ui::spacing(
            8,
            buttons());
    });

    return app.run(args);
}

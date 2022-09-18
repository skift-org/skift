#include <karm-main/main.h>
#include <karm-media/icon.h>
#include <karm-ui/app.h>
#include <karm-ui/view.h>

struct FontApp : public Ui::View<FontApp> {
    Media::Icon _icon;

    FontApp(Media::Icon icon)
        : _icon{icon} {}

    void paint(Gfx::Context &g, Math::Recti) override {
        g.fillStyle(Gfx::ZINC700);
        g.fill(bound().center() - _icon.bound().center().cast<int>(), _icon);

        g.strokeStyle(Gfx::stroke(Gfx::ZINC400).withWidth(2).withAlign(Gfx::StrokeAlign::OUTSIDE_ALIGN));
        g.stroke(bound().center() - _icon.bound().center().cast<int>(), _icon);
    }
};

CliResult entryPoint(CliArgs args) {
    Media::Icon icon = {Media::Icons::CAT, 256};

    if (args.len() == 1) {
        icon = try$(Media::Icon::byName(args[0], 256));
    }
    return Ui::runApp<FontApp>(args, icon);
}

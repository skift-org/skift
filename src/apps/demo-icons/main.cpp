#include <karm-main/main.h>
#include <karm-media/icon.h>
#include <karm-ui/app.h>

struct FontApp : public Ui::Widget<FontApp> {
    Media::Icon _icon;

    FontApp(Media::Icon icon)
        : _icon{icon} {}

    void paint(Gfx::Context &g) override {
        g.clear(Gfx::BLACK);
        g.fillStyle(Gfx::BLUE500);
        g.fill({0, 0}, _icon);
        g._rect(_icon.bound().cast<int>(), Gfx::RED500);
    }
};

CliResult entryPoint(CliArgs args) {
    Media::Icon icon = {Media::Icons::CAT, 64};

    /*
    // FIXME: clang chokes on Icon::byName()
    if (args.len() == 1) {
        icon = try$(Media::Icon::byName(args[0], 64));
    }
    */
    return Ui::runApp<FontApp>(args, icon);
}

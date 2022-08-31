#include <karm-main/main.h>
#include <karm-media/loader.h>
#include <karm-ui/app.h>
#include <png/spec.h>

struct FontApp : public Ui::Widget<FontApp> {
    Media::Image _image;

    FontApp(Media::Image image)
        : _image{image} {}

    void paint(Gfx::Context &g, Math::Recti) override {
        g.clear(Gfx::BLACK);

        g.fillStyle(Gfx::BLUE500);

        g.fill({32, 32}, "Hello, world!");
    }
};

CliResult entryPoint(CliArgs args) {
    auto image = try$(Media::loadImage(args[0]));
    return Ui::runApp<FontApp>(args, image);
}

#include <karm-main/main.h>
#include <karm-media/font-ttf.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>
#include <karm-ui/app.h>
#include <karm-ui/funcs.h>

struct FontApp : public Ui::Widget<FontApp> {
    bool _trace{false};
    Strong<Media::Font> _font;

    FontApp(Strong<Media::Font> font) : _font{font} {
    }

    void paint(Gfx::Context &g) override {
        g.clear(Gfx::BLACK);

        g.textStyle(Gfx::text().withFont(_font));
        g.fillStr({16, 100}, "Hello, world!");

        if (_trace)
            g._trace();
    }

    void event(Events::Event &e) override {
        e.handle<Events::MouseEvent>([&](auto const &m) {
            if (m.type == Events::MouseEvent::RELEASE) {
                _trace = !_trace;
                Ui::shouldRepaint(*this);
            }
            return true;
        });
    }
};

CliResult entryPoint(CliArgs args) {
    auto file = try$(Sys::File::open(args[0]));
    auto map = try$(Sys::mmap().map(file));
    auto font = try$(Media::TtfFont::load(std::move(map)));

    return Ui::runApp<FontApp>(args, font);
}

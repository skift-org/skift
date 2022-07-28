#include <karm-app/host.h>
#include <karm-main/main.h>

struct StrokeClient : public App::Client {
    bool _trace{false};
    Math::Vec2i _mousePos{300, 300};

    void paint(Gfx::Context &g) override {

        g.clear(Gfx::ZINC900);

        g.begin();
        g.moveTo({200, 200});
        g.lineTo(_mousePos.cast<double>());
        g.lineTo({200, 600});

        g.strokeStyle(
            Gfx::stroke(Gfx::BLUE500)
                .withAlign(Gfx::CENTER_ALIGN)
                .withJoin(Gfx::ROUND_JOIN)
                .withCap(Gfx::ROUND_CAP)
                .withWidth(72));
        g.stroke();

        if (_trace)
            g._trace();

        g.strokeStyle(
            Gfx::stroke(Gfx::ZINC900)
                .withAlign(Gfx::CENTER_ALIGN)
                .withJoin(Gfx::BEVEL_JOIN)
                .withCap(Gfx::BUTT_CAP)
                .withWidth(4));

        g.stroke();
    }

    void handle(Events::Event &e) override {
        e.handle<Events::MouseEvent>([&](auto const &m) {
            if (m.type == Events::MouseEvent::RELEASE) {
                _trace = !_trace;
            }
            _mousePos = m.pos;
            return true;
        });
    }
};

ExitCode entryPoint(CliArgs const &) {
    return App::run<StrokeClient>();
}

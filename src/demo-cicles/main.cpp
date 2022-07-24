#include <karm-app/host.h>
#include <karm-main/main.h>
#include <karm-math/rand.h>

struct CircleClient : public Karm::App::Client {
    Math::Rand _rand{};
    Math::Vec2i _mousePos{};

    void paint(Gfx::Context &g) override {
        double size = _rand.nextInt(4, 10);
        size *= size;

        g.begin();
        g.ellipse({
            _rand.nextVec2(g.clip()).cast<double>(),
            size,
        });
        g.fillStyle(Gfx::COLORS[_rand.nextInt(219)]);
        g.fill();

        g.begin();
        g.ellipse({_mousePos.cast<double>(), 100});
        g.fillStyle(Gfx::WHITE);
        g.fill();
    }

    void handle(Events::Event &e) override {
        e.handle<Events::MouseEvent>([&](auto &e) {
            _mousePos = e.pos;
            return true;
        });
    }
};

ExitCode entryPoint(CliArgs const &) {
    return App::run<CircleClient>();
}

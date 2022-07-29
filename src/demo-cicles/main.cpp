#include <karm-app/host.h>
#include <karm-main/main.h>
#include <karm-math/rand.h>

struct CircleClient : public Karm::App::Client {
    Math::Rand _rand{};
    Math::Vec2i _mousePos{};
    int _frame = 0;

    void onPaint(Gfx::Context &g) override {
        double size = _rand.nextInt(4, 10);
        size *= size;

        g.fillStyle(Gfx::BLACK.withOpacity(0.01));
        g.fill(g.clip());

        if (_frame % 16 == 0) {
            g.begin();
            g.ellipse({
                _rand.nextVec2(g.clip()).cast<double>(),
                size,
            });

            g.strokeStyle(
                Gfx::stroke(Gfx::WHITE)
                    .withWidth(_rand.nextInt(2, size)));
            g.stroke();
        }

        g.begin();
        g.ellipse({_mousePos.cast<double>(), 100});
        g.fillStyle(Gfx::BLUE500);
        g.fill();

        _frame++;
    }

    void onEvent(Events::Event &e) override {
        e.handle<Events::MouseEvent>([&](auto &e) {
            if (e.type == Events::MouseEvent::MOVE) {
                _mousePos = e.pos;
                shouldRepaint();
                return true;
            }
            return false;
        });
    }
};

ExitCode entryPoint(CliArgs const &) {
    return App::run<CircleClient>();
}

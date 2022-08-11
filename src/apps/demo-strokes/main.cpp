#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/funcs.h>

struct StrokesApp : public Ui::Widget<StrokesApp> {
    Math::Vec2i _mousePos{};
    bool _trace = false;

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

    void event(Events::Event &e) override {
        e
            .handle<Events::MouseEvent>([&](auto &e) {
                if (e.type == Events::MouseEvent::MOVE) {
                    _mousePos = e.pos;
                    Ui::shouldAnimate(*this);
                    return true;
                } else if (e.type == Events::MouseEvent::RELEASE) {
                    _trace = !_trace;
                    Ui::shouldAnimate(*this);
                    return true;
                }
                return true;
            })
            .handle<Events::AnimateEvent>([&](auto &) {
                Ui::shouldRepaint(*this);
                return true;
            });
    }
};

CliResult entryPoint(CliArgs args) {
    return Ui::runApp<StrokesApp>(args);
}

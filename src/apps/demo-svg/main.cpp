#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/funcs.h>

struct SvgApp : public Ui::Widget<SvgApp> {
    bool _trace{false};

    void paint(Gfx::Context &g, Math::Recti) override {
        g.clear(Gfx::BLACK);
        g.begin();

        if (!g.evalSvg("M192 128 170.72 129.44C156.96 113.12 118.4 72 80 72 80 72 48.48 119.36 79.36 182.56 70.56 195.84 65.12 202.72 64 218.56L33.12 223.2 36.48 238.88 64.64 234.72 66.88 246.08 41.76 261.12 49.28 275.36 72.48 261.12C90.88 300.16 137.44 320 192 320 246.56 320 293.12 300.16 311.52 261.12L334.72 275.36 342.24 261.12 317.12 246.08 319.36 234.72 347.52 238.88 350.88 223.2 320 218.56C318.88 202.72 313.44 195.84 304.64 182.56 335.52 119.36 304 72 304 72 265.6 72 227.04 113.12 213.28 129.44L192 128M144 176A16 16 90 01160 192 16 16 90 01144 208 16 16 90 01128 192 16 16 90 01144 176M240 176A16 16 90 01256 192 16 16 90 01240 208 16 16 90 01224 192 16 16 90 01240 176M176 224H208L196.8 246.24C200 256.48 208.96 264 220 264A24 24 90 00244 240H252A32 32 90 01220 272C208 272 197.6 265.44 192 256V256H192C186.4 265.44 176 272 164 272A32 32 90 01132 240H140A24 24 90 00164 264C175.04 264 184 256.48 187.2 246.24L176 224Z")) {
            panic("Failed to parse SVG");
        }

        g.fillStyle(Gfx::BLUE500);
        g.fill();

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
    return Ui::runApp<SvgApp>(args);
}

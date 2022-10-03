#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

struct Demo {
    Media::Icons icon;
    Str name;
    Str description;
    Ui::Build build;
};

static Array demos = {
    Demo{
        Media::Icons::SVG,
        "SVG",
        "SVG rendering",
        []() {
            return Ui::canvas(
                [](Gfx::Context &g, ...) {
                    g.begin();

                    if (!g.evalSvg("M192 128 170.72 129.44C156.96 113.12 118.4 72 80 72 80 72 48.48 119.36 79.36 182.56 70.56 195.84 65.12 202.72 64 218.56L33.12 223.2 36.48 238.88 64.64 234.72 66.88 246.08 41.76 261.12 49.28 275.36 72.48 261.12C90.88 300.16 137.44 320 192 320 246.56 320 293.12 300.16 311.52 261.12L334.72 275.36 342.24 261.12 317.12 246.08 319.36 234.72 347.52 238.88 350.88 223.2 320 218.56C318.88 202.72 313.44 195.84 304.64 182.56 335.52 119.36 304 72 304 72 265.6 72 227.04 113.12 213.28 129.44L192 128M144 176A16 16 90 01160 192 16 16 90 01144 208 16 16 90 01128 192 16 16 90 01144 176M240 176A16 16 90 01256 192 16 16 90 01240 208 16 16 90 01224 192 16 16 90 01240 176M176 224H208L196.8 246.24C200 256.48 208.96 264 220 264A24 24 90 00244 240H252A32 32 90 01220 272C208 272 197.6 265.44 192 256V256H192C186.4 265.44 176 272 164 272A32 32 90 01132 240H140A24 24 90 00164 264C175.04 264 184 256.48 187.2 246.24L176 224Z")) {
                        panic("Failed to parse SVG");
                    }

                    g.fillStyle(Gfx::ZINC700);
                    g.fill();

                    g.strokeStyle(Gfx::stroke(Gfx::ZINC400).withWidth(2));
                    g.stroke();
                });
        },
    },
    Demo{
        Media::Icons::VECTOR_LINE,
        "Strokes",
        "Strokes styling and rendering",
        []() {
            return Ui::canvas(
                [](Gfx::Context &g, ...) {
                    g.begin();
                    g.moveTo({0, 0});
                    g.lineTo({100, 100});
                    g.lineTo({200, 0});

                    g.strokeStyle(Gfx::stroke(Gfx::ZINC400).withWidth(2));
                    g.stroke();
                });
        },
    },
    Demo{
        Media::Icons::COFFEE,
        "Icons",
        "Icons rendering",
        []() {
            return Ui::canvas(
                [](Gfx::Context &g, Math::Vec2i) {
                    Media::Icon icon = {Media::Icons::COFFEE, 256};

                    g.fillStyle(Gfx::ZINC700);
                    g.fill(0, icon);

                    g.strokeStyle(Gfx::stroke(Gfx::ZINC400).withWidth(2).withAlign(Gfx::StrokeAlign::OUTSIDE_ALIGN));
                    g.stroke(0, icon);
                });
        },
    },
};

struct State {
    size_t current = 0;
};

struct SwitchAction {
    size_t index;
};

using Actions = Var<SwitchAction>;

State reduce(State, Actions action) {
    return action.visit(
        [&](SwitchAction action) {
            return State{action.index};
        });
}

using Model = Ui::Model<State, Actions>;

Ui::Child sidebar() {
    Ui::Children items =
        iter(demos)
            .mapi([](size_t index, Demo const &demo) {
                return Ui::buttonRow(Model::bind<SwitchAction>(index), demo.icon, demo.name, demo.description);
            })
            .collect<Ui::Children>();

    return Ui::vscroll(
        Ui::spacing(
            {0, 8},
            Ui::vflow(8, items)));
}

CliResult entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(
        Media::Icons::DUCK,
        "Demos",
        Ui::TitlebarStyle::DEFAULT);

    auto content = Ui::reducer<Model>(reduce, [](State s) {
        return Ui::hflow(
            8,
            sidebar(),
            Ui::separator(),
            Ui::spacing(8, Ui::vflow(Ui::text(16, demos[s.current].name), demos[s.current].build())));
    });

    auto layout = Ui::dialogLayer(
        Ui::minSize(
            420,
            Ui::vflow(
                titlebar,
                Ui::separator(),
                content)));

    return Ui::runApp(args, layout);
}

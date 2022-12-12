#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/react.h>
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

                    if (not g.evalSvg("M192 128 170.72 129.44C156.96 113.12 118.4 72 80 72 80 72 48.48 119.36 79.36 182.56 70.56 195.84 65.12 202.72 64 218.56L33.12 223.2 36.48 238.88 64.64 234.72 66.88 246.08 41.76 261.12 49.28 275.36 72.48 261.12C90.88 300.16 137.44 320 192 320 246.56 320 293.12 300.16 311.52 261.12L334.72 275.36 342.24 261.12 317.12 246.08 319.36 234.72 347.52 238.88 350.88 223.2 320 218.56C318.88 202.72 313.44 195.84 304.64 182.56 335.52 119.36 304 72 304 72 265.6 72 227.04 113.12 213.28 129.44L192 128M144 176A16 16 90 01160 192 16 16 90 01144 208 16 16 90 01128 192 16 16 90 01144 176M240 176A16 16 90 01256 192 16 16 90 01240 208 16 16 90 01224 192 16 16 90 01240 176M176 224H208L196.8 246.24C200 256.48 208.96 264 220 264A24 24 90 00244 240H252A32 32 90 01220 272C208 272 197.6 265.44 192 256V256H192C186.4 265.44 176 272 164 272A32 32 90 01132 240H140A24 24 90 00164 264C175.04 264 184 256.48 187.2 246.24L176 224Z")) {
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
                    g.translate({100, 100});
                    g.moveTo({0, 0});
                    g.lineTo({100, 100});
                    g.lineTo({200, 0});

                    g.strokeStyle(Gfx::stroke(Gfx::RED)
                                      .withCap(Gfx::StrokeCap::SQUARE_CAP)
                                      .withJoin(Gfx::StrokeJoin::MITER_JOIN)
                                      .withWidth(64));
                    g.stroke();

                    g.strokeStyle(Gfx::stroke(Gfx::BLUE)
                                      .withCap(Gfx::StrokeCap::ROUND_CAP)
                                      .withJoin(Gfx::StrokeJoin::ROUND_JOIN)
                                      .withWidth(64));
                    g.stroke();

                    g.strokeStyle(Gfx::stroke(Gfx::GREEN).withWidth(64));
                    g.stroke();
                });
        },
    },
    Demo{
        Media::Icons::CIRCLE,
        "Circles",
        "Circles rendering",
        []() {
            return Ui::canvas(
                [](Gfx::Context &g, Math::Vec2i size) {
                    Math::Rand rand{};

                    for (int i = 0; i < 10; i++) {
                        double s = rand.nextInt(4, 10);
                        s *= s;

                        g.begin();
                        g.ellipse({
                            rand.nextVec2(size).cast<double>(),
                            s,
                        });

                        g.strokeStyle(
                            Gfx::stroke(Gfx::randomColor(rand))
                                .withWidth(rand.nextInt(2, s)));
                        g.stroke();
                    }
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
    Demo{
        Media::Icons::GRADIENT_HORIZONTAL,
        "Gradients",
        "Gradients rendering",
        []() {
            return Ui::canvas(
                [](Gfx::Context &g, Math::Vec2i bound) {
                    // Red, orange, yellow, green, blue, indigo, violet
                    auto transFlag = Gfx::Gradient::linear()
                                         .withColors(Gfx::RED, Gfx::ORANGE, Gfx::YELLOW, Gfx::GREEN, Gfx::BLUE, Gfx::INDIGO, Gfx::VIOLET);
                    g.fillStyle(transFlag);
                    // Media::Icon icon = {Media::Icons::CAT, 256};
                    g.fill(bound);

                    g.fillStyle(Gfx::WHITE);
                    g.fill({16, 26}, "Graphic design is my passion");
                });
        },
    },
    Demo{
        Media::Icons::HAND_WAVE,
        "Hello, world!",
        "Hello, world!",
        []() {
            return Ui::grow(Ui::center(Ui::text(Ui::TextStyle::titleMedium(), "Hello, world!")));
        },
    },
    Demo{
        Media::Icons::LIST_BOX_OUTLINE,
        "Inputs",
        "Form inputs widgets",
        []() {
            auto button = Ui::buttonRow(
                [](auto &) {
                },
                "Cool duck app", "Install");

            auto toggle = Ui::toggleRow(true, NONE, "Some property");
            auto toggle1 = Ui::toggleRow(true, NONE, "Some property");
            auto toggle2 = Ui::toggleRow(true, NONE, "Some property");

            auto checkbox = Ui::checkboxRow(true, NONE, "Some property");
            auto checkbox1 = Ui::checkboxRow(false, NONE, "Some property");
            auto checkbox2 = Ui::checkboxRow(false, NONE, "Some property");

            auto radio = Ui::radioRow(true, NONE, "Some property");
            auto radio1 = Ui::radioRow(false, NONE, "Some property");
            auto radio2 = Ui::radioRow(false, NONE, "Some property");

            auto slider = Ui::sliderRow(0.5, NONE, "Some property");

            auto title = Ui::titleRow("Some Settings");

            auto list = Ui::card(
                button,
                Ui::separator(),
                Ui::treeRow(Ui::icon(Media::Icons::TOGGLE_SWITCH), "Switches", NONE,
                            {
                                toggle,
                                toggle1,
                                toggle2,
                            }),

                Ui::separator(),
                Ui::treeRow(Ui::icon(Media::Icons::CHECKBOX_MARKED), "Checkboxs", NONE,
                            {
                                checkbox,
                                checkbox1,
                                checkbox2,
                            }),

                Ui::separator(),
                Ui::treeRow(Ui::icon(Media::Icons::RADIOBOX_MARKED), "Radios", NONE,
                            {
                                radio,
                                radio1,
                                radio2,
                            }),

                Ui::separator(),
                slider);

            return Ui::vscroll(
                Ui::hcenter(
                    Ui::grow(
                        Ui::maxSize(
                            {420, Ui::UNCONSTRAINED},
                            Ui::vflow(8, title, list)))));
        },
    },
    Demo{
        Media::Icons::TEXT,
        "Typography",
        "Typography",
        []() {
            return Ui::vflow(
                Ui::treeRow(Ui::icon(Media::Icons::TEXT), "Display", NONE,
                            {
                                Ui::text(Ui::TextStyle::displayLarge(), "Display Large"),
                                Ui::text(Ui::TextStyle::displayMedium(), "Display Medium"),
                                Ui::text(Ui::TextStyle::displaySmall(), "Display Small"),
                            }),

                Ui::treeRow(Ui::icon(Media::Icons::TEXT), "Headlines", NONE,
                            {
                                Ui::text(Ui::TextStyle::headlineLarge(), "Headline Large"),
                                Ui::text(Ui::TextStyle::headlineMedium(), "Headline Medium"),
                                Ui::text(Ui::TextStyle::headlineSmall(), "Headline Small"),
                            }),

                Ui::treeRow(Ui::icon(Media::Icons::TEXT), "Titles", NONE,
                            {
                                Ui::text(Ui::TextStyle::titleLarge(), "Title Large"),
                                Ui::text(Ui::TextStyle::titleMedium(), "Title Medium"),
                                Ui::text(Ui::TextStyle::titleSmall(), "Title Small"),
                            }),

                Ui::treeRow(Ui::icon(Media::Icons::TEXT), "Body", NONE,
                            {
                                Ui::text(Ui::TextStyle::bodyLarge(), "Body Large"),
                                Ui::text(Ui::TextStyle::bodyMedium(), "Body Medium"),
                                Ui::text(Ui::TextStyle::bodySmall(), "Body Small"),
                            }));
        },
    },
    Demo{
        Media::Icons::COUNTER,
        "Counter",
        "Counter demo",
        []() {
            return Ui::state(1, [](auto state) {
                return Ui::vflow(
                    Ui::button(
                        [state](Ui::Node &) mutable {
                            state.update(state.value() + 1);
                        },
                        Media::Icons::PLUS),
                    Ui::center(
                        Ui::spacing(
                            12,
                            Ui::text("{}", state.value()))),
                    Ui::button(
                        [state](Ui::Node &) mutable {
                            state.update(state.value() - 1);
                        },
                        Media::Icons::MINUS));
            });
        },
    }};

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

Ui::Child sidebar(State s) {
    Ui::Children items =
        iter(demos)
            .mapi([&](size_t index, Demo const &demo) {
                return Ui::navRow(index == s.current, Model::bind<SwitchAction>(index), demo.icon, demo.name);
            })
            .collect<Ui::Children>();

    return Ui::vscroll(
        Ui::minSize({256, Ui::UNCONSTRAINED},
                    Ui::spacing(
                        8,
                        Ui::vflow(8, items))));
}

CliResult entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(
        Media::Icons::DUCK,
        "Demos",
        Ui::TitlebarStyle::DEFAULT);

    auto content = Ui::reducer<Model>(reduce, [](State s) {
        return Ui::hflow(
            sidebar(s),
            Ui::separator(),
            Ui::grow(demos[s.current].build()));
    });

    auto layout =
        Ui::minSize(
            {700, 500},
            Ui::dialogLayer(
                Ui::vflow(
                    titlebar,
                    Ui::separator(),
                    Ui::grow(content))));

    return Ui::runApp(args, layout);
}

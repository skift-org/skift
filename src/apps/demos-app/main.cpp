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
    Mdi::Icon icon;
    Str name;
    Str description;
    Ui::Build build;
};

static constexpr Str CAT = {
#include "cat.path"
};

static Array demos = {
    Demo{
        Mdi::SVG,
        "SVG",
        "SVG rendering",
        []() {
            return Ui::canvas(
                [](Gfx::Context &g, ...) {
                    g.begin();
                    g.evalSvg(CAT);

                    g.fillStyle(Ui::GRAY700);
                    g.fill();

                    g.strokeStyle(Gfx::stroke(Gfx::BLUE).withWidth(1).withAlign(Gfx::OUTSIDE_ALIGN));
                    g.stroke();
                });
        },
    },
    Demo{
        Mdi::VECTOR_LINE,
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
        Mdi::CIRCLE,
        "Circles",
        "Circles rendering",
        []() {
            return Ui::canvas(
                [](Gfx::Context &g, Math::Vec2i size) {
                    Math::Rand rand{};

                    for (isize i = 0; i < 10; i++) {
                        f64 s = rand.nextInt(4, 10);
                        s *= s;

                        g.begin();
                        g.ellipse({
                            rand.nextVec2(size).cast<f64>(),
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
        Mdi::COFFEE,
        "Icons",
        "Icons rendering",
        []() {
            return Ui::canvas(
                [](Gfx::Context &g, Math::Vec2i) {
                    Media::Icon icon = {Mdi::COFFEE, 256};

                    g.fillStyle(Ui::GRAY700);
                    g.fill(0, icon);

                    g.strokeStyle(Gfx::stroke(Ui::GRAY400).withWidth(2).withAlign(Gfx::StrokeAlign::OUTSIDE_ALIGN));
                    g.stroke(0, icon);
                });
        },
    },
    Demo{
        Mdi::GRADIENT_HORIZONTAL,
        "Gradients",
        "Gradients rendering",
        []() {
            return Ui::canvas(
                [](Gfx::Context &g, Math::Vec2i bound) {
                    // Red, orange, yellow, green, blue, indigo, violet
                    auto colors = Gfx::Gradient::linear()
                                      .withColors(Gfx::RED, Gfx::ORANGE, Gfx::YELLOW, Gfx::GREEN, Gfx::BLUE, Gfx::INDIGO, Gfx::VIOLET)
                                      .bake();

                    g.fillStyle(colors);
                    // Media::Icon icon = {Mdi::CAT, 256};
                    g.fill(bound);

                    g.fillStyle(Gfx::WHITE);
                    g.fill({16, 26}, "Graphic design is my passion");
                });
        },
    },
    Demo{
        Mdi::HAND_WAVE,
        "Hello, world!",
        "Hello, world!",
        []() {
            return Ui::grow(Ui::center(Ui::text(Ui::TextStyle::titleMedium(), "Hello, world!")));
        },
    },
    Demo{
        Mdi::LIST_BOX_OUTLINE,
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
                Ui::treeRow(Ui::icon(Mdi::TOGGLE_SWITCH), "Switches", NONE,
                            {
                                toggle,
                                toggle1,
                                toggle2,
                            }),

                Ui::separator(),
                Ui::treeRow(Ui::icon(Mdi::CHECKBOX_MARKED), "Checkboxs", NONE,
                            {
                                checkbox,
                                checkbox1,
                                checkbox2,
                            }),

                Ui::separator(),
                Ui::treeRow(Ui::icon(Mdi::RADIOBOX_MARKED), "Radios", NONE,
                            {
                                radio,
                                radio1,
                                radio2,
                            }),

                Ui::separator(),
                slider);

            return Ui::vflow(8, title, list) |
                   Ui::maxSize({420, Ui::UNCONSTRAINED}) |
                   Ui::grow() |
                   Ui::hcenter() |
                   Ui::vscroll();
        },
    },
    Demo{
        Mdi::TEXT,
        "Typography",
        "Typography",
        []() {
            return Ui::vflow(
                Ui::treeRow(Ui::icon(Mdi::TEXT), "Display", NONE,
                            {
                                Ui::text(Ui::TextStyle::displayLarge(), "Display Large"),
                                Ui::text(Ui::TextStyle::displayMedium(), "Display Medium"),
                                Ui::text(Ui::TextStyle::displaySmall(), "Display Small"),
                            }),

                Ui::treeRow(Ui::icon(Mdi::TEXT), "Headlines", NONE,
                            {
                                Ui::text(Ui::TextStyle::headlineLarge(), "Headline Large"),
                                Ui::text(Ui::TextStyle::headlineMedium(), "Headline Medium"),
                                Ui::text(Ui::TextStyle::headlineSmall(), "Headline Small"),
                            }),

                Ui::treeRow(Ui::icon(Mdi::TEXT), "Titles", NONE,
                            {
                                Ui::text(Ui::TextStyle::titleLarge(), "Title Large"),
                                Ui::text(Ui::TextStyle::titleMedium(), "Title Medium"),
                                Ui::text(Ui::TextStyle::titleSmall(), "Title Small"),
                            }),

                Ui::treeRow(Ui::icon(Mdi::TEXT), "Body", NONE,
                            {
                                Ui::text(Ui::TextStyle::bodyLarge(), "Body Large"),
                                Ui::text(Ui::TextStyle::bodyMedium(), "Body Medium"),
                                Ui::text(Ui::TextStyle::bodySmall(), "Body Small"),
                            }));
        },
    },
    Demo{
        Mdi::COUNTER,
        "Counter",
        "Counter demo",
        []() {
            return Ui::state(1, [](auto state) {
                return Ui::vflow(
                    Ui::button(
                        [state](Ui::Node &) mutable {
                            state.update(state.value() + 1);
                        },
                        Mdi::PLUS),
                    Ui::center(
                        Ui::spacing(
                            12,
                            Ui::text("{}", state.value()))),
                    Ui::button(
                        [state](Ui::Node &) mutable {
                            state.update(state.value() - 1);
                        },
                        Mdi::MINUS));
            });
        },
    }};

struct State {
    usize current = 0;
};

struct SwitchAction {
    usize index;
};

using Actions = Var<SwitchAction>;

State reduce(State, Actions action) {
    return action.visit(
        [&](SwitchAction action) {
            return State{action.index};
        });
}

using Model = Ui::Model<State, Actions, reduce>;

Ui::Child sidebar(State s) {
    Ui::Children items =
        iter(demos)
            .mapi([&](Demo const &demo, usize index) {
                return Ui::navRow(index == s.current, Model::bind<SwitchAction>(index), demo.icon, demo.name);
            })
            .collect<Ui::Children>();

    return Ui::navList(items);
}

Res<> entryPoint(Ctx &ctx) {
    auto titlebar = Ui::titlebar(
        Mdi::DUCK,
        "Demos",
        Ui::TitlebarStyle::DEFAULT);

    auto content = Ui::reducer<Model>([](State s) {
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

    return Ui::runApp(ctx, layout);
}

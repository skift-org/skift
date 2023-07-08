#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/react.h>
#include <karm-ui/row.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "demo-circle.h"
#include "demo-gradient.h"
#include "demo-hello.h"
#include "demo-icons.h"
#include "demo-inputs.h"
#include "demo-stroke.h"
#include "demo-svg.h"
#include "demo-text.h"

namespace Hideo::Demos {

static Array DEMOS = {
    &CIRCLE_DEMO,
    &GRADIENT_DEMO,
    &HELLO_DEMO,
    &ICONS_DEMO,
    &INPUTS_DEMO,
    &STROKE_DEMO,
    &SVG_DEMO,
    &TEXT_DEMO,
};

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
        iter(DEMOS)
            .mapi([&](Demo const *demo, usize index) {
                return Ui::navRow(
                    index == s.current,
                    Model::bind<SwitchAction>(index),
                    demo->icon,
                    demo->name);
            })
            .collect<Ui::Children>();

    return Ui::navList(items);
}

Ui::Child app() {
    auto titlebar = Ui::titlebar(
        Mdi::DUCK,
        "Demos",
        Ui::TitlebarStyle::DEFAULT);

    auto content = Ui::reducer<Model>([](State s) {
        return Ui::hflow(
            sidebar(s),
            Ui::separator(),
            DEMOS[s.current]->build() | Ui::grow());
    });

    return Ui::minSize(
        {700, 500},
        Ui::dialogLayer(
            Ui::vflow(
                titlebar,
                Ui::separator(),
                content | Ui::grow())));
}

} // namespace Hideo::Demos

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Demos::app());
}

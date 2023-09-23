#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
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

namespace Demos {

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

using Action = Var<SwitchAction>;

void reduce(State &s, Action action) {
    action.visit(
        [&](SwitchAction action) {
            s.current = action.index;
        });
}

using Model = Ui::Model<State, Action, reduce>;

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
    return Ui::reducer<Model>([](State s) {
        return Ui::scafold({
            .icon = Mdi::DUCK,
            .title = "Demos",
            .sidebar = sidebar(s),
            .body = DEMOS[s.current]->build(),
        });
    });
}

} // namespace Demos

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Demos::app());
}

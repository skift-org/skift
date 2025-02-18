#include <karm-kira/scaffold.h>
#include <karm-kira/side-nav.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>
#include <mdi/duck.h>

#include "demo-bezier.h"
#include "demo-carousel.h"
#include "demo-circle.h"
#include "demo-gradient.h"
#include "demo-hello.h"
#include "demo-mixbox.h"
#include "demo-stroke.h"
#include "demo-svg.h"
#include "demo-text.h"

namespace Hideo::Demos {

static Array DEMOS = {
    &BEZIER_DEMO,
    &CAROUSEL_DEMO,
    &CIRCLE_DEMO,
    &GRADIENT_DEMO,
    &HELLO_DEMO,
    &MIXBOX_DEMO,
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

using Action = Union<SwitchAction>;

Ui::Task<Action> reduce(State& s, Action action) {
    action.visit(
        [&](SwitchAction action) {
            s.current = action.index;
        }
    );

    return NONE;
}

using Model = Ui::Model<State, Action, reduce>;

Ui::Child app() {
    return Ui::reducer<Model>([](State const& s) {
        return Kr::scaffold({
            .icon = Mdi::DUCK,
            .title = "Demos"s,
            .sidebar = [&] {
                return Kr::sidenav(
                    iter(DEMOS)
                        .mapi([&](Demo const* demo, usize index) {
                            return Kr::sidenavItem(
                                index == s.current,
                                Model::bind<SwitchAction>(index),
                                demo->icon,
                                demo->name
                            );
                        })
                        .collect<Ui::Children>()
                );
            },
            .body = [&] {
                return DEMOS[s.current]->build() | Ui::key(s.current);
            },
        });
    });
}

} // namespace Hideo::Demos

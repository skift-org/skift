#include "model.h"

namespace Hideo::Sysmon {

Ui::Task<Action> reduce(State &s, Action a) {
    if (auto goTo = a.is<GoTo>())
        s.tab = goTo->tab;

    return NONE;
}

} // namespace Hideo::Sysmon

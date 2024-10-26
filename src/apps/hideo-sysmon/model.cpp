#include "model.h"

namespace Hideo::Sysmon {

void reduce(State &s, Action a) {
    if (auto goTo = a.is<GoTo>())
        s.tab = goTo->tab;
}

} // namespace Hideo::Sysmon

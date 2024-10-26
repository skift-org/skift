#include "app.h"

namespace Hideo::Spreadsheet {

void reduce(State &s, Action a) {
    a.visit(
        Visitor{
            [&](UpdateSelection &u) {
                s.selection = u.range;
            },
            [&](ToggleProperties &) {
                s.propertiesVisible = not s.propertiesVisible;
            },
            [&](SwitchSheet &u) {
                s.active = u.index;
                s.selection = NONE;
            },
            [&](auto &) {
                debug("Unhandled action");
            },
        }
    );

    s.activeSheet().recompute();
}

} // namespace Hideo::Spreadsheet

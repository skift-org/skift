#include "model.h"

namespace Hideo::Colors {

void reduce(State &s, Action action) {
    action.visit(Visitor{
        [&](UpdatePage update) {
            s.page = update.page;
        },
        [&](UpdateHsv update) {
            s.hsv = update.hsv;
        },
    });
}

} // namespace Hideo::Colors

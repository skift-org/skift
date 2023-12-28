#include "model.h"

namespace Hideo::Clock {

void reduce(State &s, Action a) {
    a.visit(Visitor{
        [&](Page p) {
            s.page = p;
        },
    });
}

} // namespace Hideo::Clock

#include <karm-sys/time.h>

#include "model.h"

namespace Hideo::Clock {

void reduce(State &s, Action a) {
    a.visit(Visitor{
        [&](Page p) {
            s.page = p;
        },
        [&](TimeTick) {
            s.dateTime = Sys::dateTime();
        }
    });
}

} // namespace Hideo::Clock

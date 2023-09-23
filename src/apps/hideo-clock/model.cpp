#include "model.h"

namespace Clock {

void reduce(State &s, Action a) {
    a.visit(Visitor{
        [&](Page p) {
            s.page = p;
        },
    });
}

} // namespace Clock

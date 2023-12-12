#include "app.h"

namespace Hideo::Images {

void reduce(State &s, Action a) {
    a.visit(Visitor{
        [&](Refresh) {
        },
        [&](ToggleEditor) {
            s.filter = Gfx::Unfiltered{};
            s.isEditor = !s.isEditor;
        },
        [&](SetFilter f) {
            s.filter = f.filter;
        },
        [&](ApplyFilter) {
        },
        [&](SaveImage) {
        },
    });
}

} // namespace Hideo::Images

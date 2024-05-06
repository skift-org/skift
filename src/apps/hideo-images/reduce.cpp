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

    if (s.isEditor and s.image) {
        computeHistogram(s.hist, s.image.unwrap());
    }
}

} // namespace Hideo::Images

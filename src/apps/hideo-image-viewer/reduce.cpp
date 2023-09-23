#include "app.h"

namespace ImageViewer {

void reduce(State &s, Action a) {
    a.visit(Visitor{
        [&](Refresh) {
            debug("Refresh");
        },
        [&](ToggleEditor) {
            s.filter = Gfx::Unfiltered{};
            s.isEditor = !s.isEditor;
        },
        [&](SetFilter f) {
            s.filter = f.filter;
        },
        [&](ApplyFilter) {
            debug("Apply filter");
        },
        [&](SaveImage) {
            debug("Save image");
        },
    });
}

} // namespace ImageViewer

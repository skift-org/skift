#include "app.h"

namespace ImageViewer {

State reduce(State s, Actions action) {
    return action.visit(Visitor{
        [&](Refresh) {
            debug("Refresh");
            return s;
        },
        [&](ToggleEditor) {
            s.filter = Gfx::Unfiltered{};
            s.isEditor = !s.isEditor;
            return s;
        },
        [&](SetFilter f) {
            s.filter = f.filter;
            return s;
        },
        [&](ApplyFilter) {
            debug("Apply filter");
            return s;
        },
        [&](SaveImage) {
            debug("Save image");
            return s;
        },
    });
}

} // namespace ImageViewer

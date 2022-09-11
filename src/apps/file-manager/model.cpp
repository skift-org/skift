#include "model.h"

namespace FileManager {

State reduce(State d, Actions action) {
    return action.visit(Visitor{
        [&](GoBack) {
            if (d.currentIndex > 0) {
                d.currentIndex--;
            }
            return d;
        },
        [&](GoForward) {
            if (d.currentIndex + 1 < d.history.len()) {
                d.currentIndex++;
            }
            return d;
        },
        [&](GoParent p) {
            auto parent = Sys::Path{d.currentPath()}.parent(p.index);
            return reduce(d, GoTo{parent});
        },
        [&](GoTo gotTo) {
            if (Op::eq(d.currentPath(), gotTo.path)) {
                return d;
            }

            Sys::Path dest = gotTo.path.isAbsolute()
                                 ? gotTo.path
                                 : Fmt::format("{}/{}", d.currentPath(), gotTo.path);

            d.history.truncate(d.currentIndex + 1);
            d.history.pushBack(dest);
            d.currentIndex++;

            return reduce(d, Refresh{});
        },
        [&](Refresh) {
            return d;
        },
        [&](AddBookmark) {
            return d;
        },
    });
}

} // namespace FileManager

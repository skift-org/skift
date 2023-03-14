#include <karm-sys/info.h>

#include "model.h"

namespace FileManager {

State reduce(State d, Actions action) {
    return action.visit(Visitor{
        [&](GoRoot) {
            return reduce(d, GoTo{Sys::Path{"/"}});
        },
        [&](GoHome) {
            auto user = Sys::userinfo().unwrap();
            return reduce(d, GoTo{user.home});
        },
        [&](GoBack) {
            if (d.canGoBack()) {
                d.currentIndex--;
            }
            return d;
        },
        [&](GoForward) {
            if (d.canGoForward()) {
                d.currentIndex++;
            }
            return d;
        },
        [&](GoParent p) {
            auto parent = d.currentPath().down(p.index);
            return reduce(d, GoTo{parent});
        },
        [&](GoTo gotTo) {
            if (Op::eq(d.currentPath(), gotTo.path)) {
                return d;
            }

            Sys::Path dest = gotTo.path.isAbsolute()
                                 ? gotTo.path
                                 : Fmt::format("{}/{}", d.currentPath(), gotTo.path).unwrap();

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

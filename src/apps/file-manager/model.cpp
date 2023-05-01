#include <karm-sys/info.h>

#include "model.h"

namespace FileManager {

State reduce(State d, Actions action) {
    return action.visit(Visitor{
        [&](GoRoot) {
            return reduce(d, GoTo{"file:/"_url});
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
            auto parent = d.currentUrl().parent(p.index);
            return reduce(d, GoTo{parent});
        },
        [&](Navigate navigate) {
            Sys::Url dest = d.currentUrl();
            dest.append(navigate.item);
            return reduce(d, GoTo{dest});
        },
        [&](GoTo gotTo) {
            if (Op::eq(d.currentUrl(), gotTo.url)) {
                return d;
            }

            d.history.truncate(d.currentIndex + 1);
            d.history.pushBack(gotTo.url);
            d.currentIndex++;

            return d;
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

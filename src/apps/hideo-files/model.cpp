#include <karm-sys/info.h>
#include <karm-sys/launch.h>

#include "model.h"

namespace Hideo::Files {

Ui::Task<Action> reduce(State& s, Action a) {
    return a.visit(Visitor{
        [&](GoRoot) {
            return reduce(s, GoTo{"file:/"_url});
        },
        [&](GoBack) {
            if (s.canGoBack())
                s.currentIndex--;
            return NONE;
        },
        [&](GoForward) {
            if (s.canGoForward())
                s.currentIndex++;
            return NONE;
        },
        [&](GoParent p) {
            auto parent = s.currentUrl().parent(p.index);
            return reduce(s, GoTo{parent});
        },
        [&](Navigate navigate) -> Ui::Task<Action> {
            auto dest = s.currentUrl();
            dest.append(navigate.item);

            auto stat = Sys::stat(dest).unwrap();
            if (stat.type == Sys::Type::FILE) {
                (void)Sys::launch({
                    .action = Mime::Uti::PUBLIC_PREVIEW,
                    .objects = {dest},
                });
            } else {
                return reduce(s, GoTo{dest});
            }
            return NONE;
        },
        [&](GoTo gotTo) {
            if (s.currentUrl() == gotTo.url)
                return NONE;

            s.history.trunc(s.currentIndex + 1);
            s.history.pushBack(gotTo.url);
            s.currentIndex++;
            return NONE;
        },
        [&](Refresh) {
            return NONE;
        },
        [&](AddBookmark) {
            return NONE;
        },
        [&](ToggleHidden) {
            s.showHidden = not s.showHidden;
            return NONE;
        },
    });
}

} // namespace Hideo::Files

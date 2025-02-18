#include <karm-sys/info.h>
#include <karm-sys/launch.h>

#include "model.h"

namespace Hideo::Files {

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(Visitor{
        [&](GoRoot) {
            reduce(s, GoTo{"file:/"_url}).unwrap();
        },
        [&](GoBack) {
            if (s.canGoBack())
                s.currentIndex--;
        },
        [&](GoForward) {
            if (s.canGoForward())
                s.currentIndex++;
        },
        [&](GoParent p) {
            auto parent = s.currentUrl().parent(p.index);
            reduce(s, GoTo{parent}).unwrap();
        },
        [&](Navigate navigate) {
            auto dest = s.currentUrl();
            dest.append(navigate.item);

            auto stat = Sys::stat(dest).unwrap();
            if (stat.type == Sys::Type::FILE) {
                (void)Sys::launch({
                    .action = Mime::Uti::PUBLIC_PREVIEW,
                    .objects = {dest},
                });
            } else {
                reduce(s, GoTo{dest}).unwrap();
            }
        },
        [&](GoTo gotTo) {
            if (s.currentUrl() == gotTo.url)
                return;

            s.history.trunc(s.currentIndex + 1);
            s.history.pushBack(gotTo.url);
            s.currentIndex++;
        },
        [&](Refresh) {
        },
        [&](AddBookmark) {
        },
        [&](ToggleHidden) {
            s.showHidden = not s.showHidden;
        },
    });

    return NONE;
}

} // namespace Hideo::Files

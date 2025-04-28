module;

#include <karm-sys/info.h>
#include <karm-sys/launch.h>

export module Hideo.Files:model;

import Karm.Ui;

namespace Hideo::Files {

struct State {
    Vec<Mime::Url> history;
    usize currentIndex = 0;
    bool showHidden = false;

    State(Mime::Url path)
        : history({path}) {}

    Mime::Url currentUrl() const {
        return history[currentIndex];
    }

    bool canGoBack() const {
        return currentIndex > 0;
    }

    bool canGoForward() const {
        return currentIndex + 1 < history.len();
    }

    bool canGoParent() const {
        return currentUrl().path.len() > 0;
    }
};

struct GoRoot {};

struct GoHome {};

struct GoBack {};

struct GoForward {};

struct GoParent {
    isize index;
};

struct GoTo {
    Mime::Url url;
};

struct Navigate {
    String item;
};

struct Refresh {
};

struct AddBookmark {};

struct ToggleHidden {};

using Action = Union<
    GoRoot,
    GoBack,
    GoForward,
    GoParent,
    Navigate,
    GoTo,
    Refresh,
    AddBookmark,
    ToggleHidden>;

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

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Files

#pragma once

#include <karm-sys/url.h>
#include <karm-ui/reducer.h>

namespace FileManager {

struct State {
    Vec<Sys::Url> history;
    usize currentIndex = 0;

    State(Sys::Url path)
        : history({path}) {}

    Sys::Url currentUrl() const {
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
    Sys::Url url;
};

struct Navigate {
    String item;
};

struct Refresh {
};

struct AddBookmark {};

using Action = Var<GoRoot, GoBack, GoForward, GoParent, Navigate, GoTo, Refresh, AddBookmark>;

void reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace FileManager

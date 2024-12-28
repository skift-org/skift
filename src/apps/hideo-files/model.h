#pragma once

#include <karm-mime/url.h>
#include <karm-ui/reducer.h>

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

using Action = Union<GoRoot, GoBack, GoForward, GoParent, Navigate, GoTo, Refresh, AddBookmark, ToggleHidden>;

Ui::Task<Action> reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Files

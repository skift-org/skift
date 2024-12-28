#pragma once

#include <karm-ui/reducer.h>

namespace Hideo::Settings {

enum struct Page {
    HOME,
    ACCOUNT,
    PERSONALIZATION,
    APPLICATIONS,
    SYSTEM,
    NETWORK,
    SECURITY,
    UPDATES,
    ABOUT,
};

struct State {
    Vec<Page> history = {Page::HOME};
    usize historyIndex = 0;

    Page page() const {
        return history[historyIndex];
    }

    bool canGoBack() const {
        return historyIndex > 0;
    }

    bool canGoForward() const {
        return historyIndex < history.len() - 1;
    }
};

struct GoTo {
    Page page;
};

struct GoBack {};

struct GoForward {};

using Action = Union<GoTo, GoBack, GoForward>;

Ui::Task<Action> reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Settings

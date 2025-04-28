module;

#include <karm-base/union.h>
#include <karm-base/vec.h>

export module Hideo.Settings:model;

import Karm.Ui;

namespace Hideo::Settings {

export enum struct Page {
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

export struct State {
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

export struct GoTo {
    Page page;
};

export struct GoBack {};

export struct GoForward {};

export using Action = Union<GoTo, GoBack, GoForward>;

Ui::Task<Action> reduce(State& s, Action action) {
    action.visit(Visitor{
        [&](GoTo a) {
            if (s.page() == a.page)
                return;

            s.history.removeRange(s.historyIndex + 1, s.history.len() - s.historyIndex - 1);
            s.history.pushBack(a.page);
            s.historyIndex = s.history.len() - 1;
        },
        [&](GoBack) {
            s.historyIndex = max(s.historyIndex - 1, 0uz);
        },
        [&](GoForward) {
            s.historyIndex = min(s.historyIndex + 1, s.history.len() - 1);
        },
    });

    return NONE;
}

export using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Settings

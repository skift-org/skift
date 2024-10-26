#pragma once

#include <karm-ui/reducer.h>

namespace Hideo::Settings {

// MARK: Model -----------------------------------------------------------------

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

void reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

// MARK: Views -----------------------------------------------------------------

Ui::Child pagescaffold(Ui::Child inner);

Ui::Child pageHome(State const &);

Ui::Child pageAbout(State const &);

} // namespace Hideo::Settings

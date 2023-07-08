#include "app.h"

namespace Settings {

State reduce(State d, Actions action) {
    return action.visit(Visitor{
        [&](ToggleSidebar) {
            d.sidebarOpen = !d.sidebarOpen;
            return d;
        },
        [&](GoTo a) {
            if (d.page() == a.page) {
                return d;
            }
            d.history.removeRange(d.historyIndex + 1, d.history.len() - d.historyIndex - 1);
            d.history.pushBack(a.page);
            d.historyIndex = d.history.len() - 1;
            return d;
        },
        [&](GoBack) {
            d.historyIndex = max(d.historyIndex - 1, 0uz);
            return d;
        },
        [&](GoForward) {
            d.historyIndex = min(d.historyIndex + 1, d.history.len() - 1);
            return d;
        },
    });
}

} // namespace Settings

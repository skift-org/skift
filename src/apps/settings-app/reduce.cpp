#include "app.h"

namespace Settings {

State reduce(State d, Actions action) {
    return action.visit(Visitor{
        [&](ToggleSidebar) {
            d.sidebarOpen = !d.sidebarOpen;
            return d;
        },
        [&](GoTo a) {
            d.page = a.page;
            return d;
        }});
}

} // namespace Settings

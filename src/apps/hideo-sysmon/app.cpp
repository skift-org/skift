#include <karm-kira/scaffold.h>
#include <karm-kira/side-nav.h>
#include <karm-ui/layout.h>

#include "app.h"
#include "model.h"

namespace Hideo::Sysmon {

// MARK: Sidebar ---------------------------------------------------------------

Ui::Child graph(Gfx::Color color) {
    return Ui::empty({72, 0}) |
           Ui::box({
               .borderRadii = 4,
               .borderWidth = 1,
               .borderFill = color,
               .backgroundFill = color.withOpacity(0.25),
           });
}

Ui::Child sidebarItem(bool selected, Ui::OnPress onPress, Gfx::Color color, String title, String description) {
    return Ui::hflow(
               4,
               graph(color),
               Ui::vflow(
                   Ui::titleSmall(title),
                   Ui::labelMedium(description)
               ) |
                   Ui::insets(4) |
                   Ui::minSize({112, Ui::UNCONSTRAINED})
           ) |
           Ui::insets(4) |
           Ui::button(
               std::move(onPress),
               selected ? Ui::ButtonStyle::secondary() : Ui::ButtonStyle::subtle()
           );
}

Ui::Child sidebar(State const &s) {
    return Kr::sidenav({
        Kr::sidenavTitle("Software"s),
        Kr::sidenavItem(
            s.tab == Tab::APPLICATIONS,
            Model::bind<GoTo>(Tab::APPLICATIONS),
            Mdi::APPS,
            "Applications"s
        ),
        Kr::sidenavItem(
            s.tab == Tab::SERVICES,
            Model::bind<GoTo>(Tab::SERVICES),
            Mdi::COG,
            "Services"s
        ),
        Kr::sidenavItem(
            s.tab == Tab::KERNEL,
            Model::bind<GoTo>(Tab::KERNEL),
            Mdi::ATOM,
            "Kernel"s
        ),

        Ui::separator(),
        Kr::sidenavTitle("Hardware"s),

        sidebarItem(
            s.tab == Tab::PROCESSORS,
            Model::bind<GoTo>(Tab::PROCESSORS),
            Gfx::BLUE,
            "Processors"s,
            "AMD Ryzen 7 7700X"s
        ),
        sidebarItem(
            s.tab == Tab::MEMORY,
            Model::bind<GoTo>(Tab::MEMORY),
            Gfx::CYAN,
            "Memory"s,
            "32GB DDR5 3200MHz"s
        ),
        sidebarItem(
            s.tab == Tab::DRIVES,
            Model::bind<GoTo>(Tab::DRIVES),
            Gfx::GREEN,
            "Drives"s,
            "1TB NVMe SSD"s
        ),
        sidebarItem(
            s.tab == Tab::NETWORK,
            Model::bind<GoTo>(Tab::NETWORK),
            Gfx::PINK,
            "Network"s,
            "10GbE NIC"s
        ),
        sidebarItem(
            s.tab == Tab::GRAPHICS,
            Model::bind<GoTo>(Tab::GRAPHICS),
            Gfx::ORANGE,
            "Graphics"s,
            "AMD Radeon RX 7900 XT"s
        ),
    });
}

// MARK: Body ------------------------------------------------------------------

// MARK: App -------------------------------------------------------------------

Ui::Child app() {
    return Ui::reducer<Model>({}, [](State const &s) {
        return Kr::scaffold({
            .icon = Mdi::VIEW_DASHBOARD,
            .title = "System Monitor"s,
            .sidebar = slot$(sidebar(s)),
            .body = slot$(Ui::empty()),
        });
    });
}

} // namespace Hideo::Tasks

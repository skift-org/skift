#include <karm-kira/scaffold.h>
#include <karm-kira/searchbar.h>
#include <karm-kira/side-nav.h>
#include <karm-ui/layout.h>
#include <mdi/basket.h>
#include <mdi/briefcase-outline.h>
#include <mdi/brush-outline.h>
#include <mdi/download-box-outline.h>
#include <mdi/gamepad-outline.h>
#include <mdi/shape.h>
#include <mdi/star-outline.h>

#include "app.h"

namespace Hideo::Apps {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::BASKET,
        .title = "Apps"s,
        .body = [] {
            return Ui::hflow(
                Kr::sidenav({
                    Kr::searchbar() | Ui::insets({6, 0}),
                    Kr::sidenavItem(true, Ui::NOP, Mdi::STAR_OUTLINE, "Discover"s),
                    Kr::sidenavItem(false, Ui::NOP, Mdi::GAMEPAD_OUTLINE, "Play"s),
                    Kr::sidenavItem(false, Ui::NOP, Mdi::BRUSH_OUTLINE, "Create"s),
                    Kr::sidenavItem(false, Ui::NOP, Mdi::BRIEFCASE_OUTLINE, "Work"s),
                    Kr::sidenavItem(false, Ui::NOP, Mdi::SHAPE, "Other"s),
                    Kr::sidenavItem(false, Ui::NOP, Mdi::DOWNLOAD_BOX_OUTLINE, "Updates"s),
                }),
                Ui::separator()
            );
        },
    });
}

} // namespace Hideo::Apps

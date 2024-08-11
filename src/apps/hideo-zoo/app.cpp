#include <hideo-base/scafold.h>

#include "app.h"
#include "model.h"

// Pages
#include "page-alert.h"
#include "page-avatar.h"
#include "page-badge.h"
#include "page-card.h"
#include "page-checkbox.h"
#include "page-color-input.h"
#include "page-context-menu.h"
#include "page-dialog.h"
#include "page-hsv-square.h"
#include "page-navbar.h"
#include "page-radio.h"
#include "page-side-nav.h"
#include "page-side-panel.h"
#include "page-slider.h"
#include "page-toggle.h"

namespace Hideo::Zoo {

static Array PAGES = {
    &PAGE_ALERT,
    &PAGE_AVATAR,
    &PAGE_BADGE,
    &PAGE_CARD,
    &PAGE_CHECKBOX,
    &PAGE_COLOR_INPUT,
    &PAGE_CONTEXT_MENU,
    &PAGE_DIALOG,
    &PAGE_HSV_SQUARE,
    &PAGE_NAVBAR,
    &PAGE_RADIO,
    &PAGE_SIDE_PANEL,
    &PAGE_SIDENAV,
    &PAGE_SLIDER,
    &PAGE_TOGGLE,
};

Ui::Child app() {
    return Ui::reducer<Model>([](State const &s) {
        return scafold({
            .icon = Mdi::DUCK,
            .title = "Zoo"s,
            .sidebar = [&] {
                return Kr::sidenav(
                    iter(PAGES)
                        .mapi([&](Page const *page, usize index) {
                            return Kr::sidenavItem(
                                index == s.page,
                                Model::bind<Switch>(index),
                                page->icon,
                                page->name
                            );
                        })
                        .collect<Ui::Children>()
                );
            },
            .body = [&] {
                auto &page = PAGES[s.page];
                return Ui::vflow(
                    Ui::vflow(
                        Ui::titleMedium(page->name),
                        Ui::empty(4),
                        Ui::bodySmall(page->description)
                    ) | Ui::insets(16),
                    Ui::separator(),
                    page->build() | Ui::grow()
                );
            },
        });
    });
}

} // namespace Hideo::Zoo

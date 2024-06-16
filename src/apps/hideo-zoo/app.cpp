#include <hideo-base/scafold.h>
#include <hideo-base/sidenav.h>

#include "app.h"
#include "model.h"
#include "page-badge.h"

namespace Hideo::Zoo {

static Array PAGES = {
    &PAGE_BADGE
};

Ui::Child app() {
    return Ui::reducer<Model>([](State const &s) {
        return scafold({
            .icon = Mdi::DUCK,
            .title = "Zoo"s,
            .sidebar = [&] {
                return Hideo::sidenav(
                    iter(PAGES)
                        .mapi([&](Page const *page, usize index) {
                            return Hideo::sidenavItem(
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
                    Ui::titleLarge(page->name),
                    Ui::bodyMedium(page->description),
                    page->build()
                );
            },
        });
    });
}

} // namespace Hideo::Zoo

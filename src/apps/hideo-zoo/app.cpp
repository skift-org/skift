export module Hideo.Zoo:app;

import Karm.Core;
import Karm.Kira;
import Karm.Ui;
import Mdi;

import :pages;
import :model;

namespace Hideo::Zoo {

export Ui::Child app() {
    return Ui::reducer<Model>([](State const& s) {
        return Kr::scaffold({
            .icon = Mdi::DUCK,
            .title = "Zoo"s,
            .sidebar = [&] {
                return Kr::sidenav(
                    iter(PAGES)
                        .mapi([&](Page const* page, usize index) {
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
                auto& page = PAGES[s.page];
                return Ui::vflow(
                    Ui::vflow(
                        Ui::titleMedium(page->name),
                        Ui::empty(4),
                        Ui::bodySmall(page->description)
                    ) | Ui::insets(16),
                    Kr::separator(),
                    page->build() | Ui::grow()
                );
            },
        });
    });
}

} // namespace Hideo::Zoo

export module Hideo.Shell:notifications;

import Mdi;
import Karm.Ui;
import Karm.Kira;
import Karm.Core;

import :model;

using namespace Karm;

namespace Hideo::Shell {

Ui::Child noti(Noti const& noti, usize i) {
    return Ui::vflow(
               8,
               Ui::hflow(
                   4,
                   Ui::icon(Mdi::INFORMATION, 12) | Ui::box({.foregroundFill = Ui::ACCENT400}),
                   Ui::text(Ui::TextStyles::labelMedium().withColor(Ui::GRAY400), "Hideo Shell")
               ),
               Ui::vflow(
                   6,
                   Ui::labelLarge(noti.title),
                   Ui::labelMedium(noti.body)
               )
           ) |
           Ui::box({
               .padding = 12,
               .borderRadii = 4,
               .backgroundFill = Ui::GRAY900,
           }) |
           Ui::dragRegion() |
           Ui::dismisable(
               Model::bind<DimisNoti>(i),
               Ui::DismisDir::HORIZONTAL,
               0.3
           ) |
           Ui::key(noti.id);
}

export Ui::Child notifications(State const& state) {
    if (not state.noti.len()) {
        return Ui::text(
                   Ui::TextStyles::labelMedium()
                       .withColor(Ui::GRAY400),
                   "No notifications"
               ) |
               Ui::center();
    }

    return Ui::vflow(
               8,
               iter(state.noti)
                   .mapi(noti)
                   .collect<Ui::Children>()
           ) |
           Ui::vscroll();
}

} // namespace Hideo::Shell

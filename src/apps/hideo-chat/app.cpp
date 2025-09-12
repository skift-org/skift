module;

#include <karm-math/align.h>

export module Hideo.Chat;

import Mdi;
import Karm.Core;
import Karm.Kira;
import Karm.Ui;

using namespace Karm;

namespace Hideo::Chat {

Ui::Child messageSidebarItem(Str name) {
    return Kr::sidenavItem(
        false,
        Ui::SINK<>,
        Ui::hflow(
            12,
            Math::Align::CENTER,
            Kr::avatar(),
            Ui::vflow(
                4,
                Ui::labelLarge(name),
                Ui::bodySmall("Some test message...")
            )
        )
    );
}

Ui::Child messageSidebar() {
    return Kr::sidenav({
        Kr::searchbar(""s) | Ui::insets({6, 0}),
        messageSidebarItem("Mathilde"),
        messageSidebarItem("Lou"),
        messageSidebarItem("Jordan"),
        messageSidebarItem("Cyp"),
        messageSidebarItem("D0p1"),
    });
}

Ui::Child messageComposer() {
    return Ui::hflow(
               4,
               Kr::input("Message"s, ""s, Ui::SINK<String>) | Ui::grow(),
               Ui::button(Ui::SINK<>, Ui::ButtonStyle::primary(), Mdi::SEND, "Send"s)
           ) |
           Ui::insets(6);
}

Ui::Child messageBubble(String text, bool self) {
    return Ui::bodyMedium(text) |
           Ui::sizing(Ui::UNCONSTRAINED, {320, Ui::UNCONSTRAINED}) |
           Ui::box({
               .margin = {2, 12},
               .padding = {6, 12},
               .borderRadii = 16,
               .backgroundFill = self ? Ui::ACCENT900 : Ui::GRAY900,
               .foregroundFill = Ui::GRAY100,
           }) |
           Ui::align(self ? Math::Align::TOP_END : Math::Align::TOP_START);
}

Ui::Child messageThread() {
    return Ui::vflow(
               Kr::separator("Yesteday"s),
               messageBubble("Yo"s, false),
               messageBubble("Yo :3"s, true),
               Kr::separator("Today"s),
               messageBubble("How are you?"s, false),
               messageBubble("Good and you?"s, true),
               messageBubble("The SNCB AM96 (also known as MS96 in Dutch) is a series of electric multiple units (EMUs) operated by the National Railway Company of Belgium (SNCB/NMBS). Introduced between 1996 and 1999, these trains were developed to modernize Belgium's intercity services and replace older rolling stock."s, false),
               messageBubble("Cool"s, true),
               Kr::separator("Yesteday"s),
               messageBubble("Yo"s, false),
               messageBubble("Yo :3"s, true),
               Kr::separator("Today"s),
               messageBubble("How are you?"s, false),
               messageBubble("Good and you?"s, true),
               messageBubble("The SNCB AM96 (also known as MS96 in Dutch) is a series of electric multiple units (EMUs) operated by the National Railway Company of Belgium (SNCB/NMBS). Introduced between 1996 and 1999, these trains were developed to modernize Belgium's intercity services and replace older rolling stock."s, false),
               messageBubble("Cool"s, true)
           ) |
           Ui::vscroll();
}

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MESSAGE,
        .title = "Chat"s,
        .sidebar = [] {
            return messageSidebar();
        },
        .body = [] {
            return Ui::vflow(
                messageThread() | Ui::grow(),
                Kr::separator(),
                messageComposer()
            );
        },
    });
}

} // namespace Hideo::Chat

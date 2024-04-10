#include <hideo-base/scafold.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <web-view/view.h>

namespace Hideo::Browser {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::WEB,
        .title = "Browser"s,
        .startTools = slots$(
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::REFRESH),
        ),
        .body = [] {
            return Web::view();
        },
    });
}

} // namespace Hideo::Browser

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Browser::app());
}

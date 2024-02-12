#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>

#include "model.h"

namespace Hideo::Counter {

Ui::Child app() {
    return Ui::reducer<Model>([](State const &s) {
        return Ui::scafold({
            .icon = Mdi::COUNTER,
            .title = "Counter",
            .titlebar = Ui::TitlebarStyle::DIALOG,
            .body = [&] {
                auto decBtn = Ui::button(
                    Model::bind<DecrementAction>(),
                    Ui::ButtonStyle::regular().withRadius(999),
                    Mdi::MINUS_THICK);

                auto incBtn = Ui::button(
                    Model::bind<IncrementAction>(),
                    Ui::ButtonStyle::regular().withRadius(999),
                    Mdi::PLUS_THICK);

                auto resetBtn = Ui::button(
                    Model::bindIf<ResetAction>(not s.initial),
                    Ui::ButtonStyle::subtle().withRadius(999),
                    Mdi::REFRESH, "Reset");

                return Ui::vflow(
                           32,
                           Layout::Align::CENTER,
                           Ui::text(
                               Ui::TextStyles::codeLarge()
                                   .withSize(48),
                               "{}", s.counter) |
                               Ui::grow(),
                           Ui::hflow(16, decBtn, incBtn),
                           resetBtn) |
                       Ui::spacing(32);
            },
            .size = 420,
        });
    });
}

} // namespace Hideo::Counter

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Counter::app());
}

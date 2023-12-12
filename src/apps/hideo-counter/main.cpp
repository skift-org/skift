#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>

#include "model.h"

namespace Hideo::Counter {

Ui::Child app() {
    auto content = Ui::reducer<Model>([](State state) {
        auto decBtn = Ui::button(
            Model::bind<DecrementAction>(),
            Ui::ButtonStyle::regular().withRadius(999),
            Mdi::MINUS_THICK);

        auto incBtn = Ui::button(
            Model::bind<IncrementAction>(),
            Ui::ButtonStyle::regular().withRadius(999),
            Mdi::PLUS_THICK);

        auto resetBtn = Ui::button(
            Model::bindIf<ResetAction>(not state.initial),
            Ui::ButtonStyle::subtle().withRadius(999),
            Mdi::REFRESH, "RESET");

        return Ui::vflow(
                   32,
                   Layout::Align::CENTER,
                   Ui::displayMedium("{}", state.counter) | Ui::grow(),
                   Ui::hflow(16, decBtn, incBtn),
                   resetBtn) |
               Ui::spacing(32);
    });

    return Ui::scafold({
        .icon = Mdi::COUNTER,
        .title = "Counter",
        .titlebar = Ui::TitlebarStyle::DIALOG,
        .body = content,
        .size = 420,
    });
}

} // namespace Hideo::Counter

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Counter::app());
}

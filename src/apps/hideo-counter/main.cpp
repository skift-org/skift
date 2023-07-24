#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/scafold.h>

#include "model.h"

namespace Counter {

Ui::Child rounedButton(Func<void(Ui::Node &)> onPress, Ui::ButtonStyle style, Media::Icon i) {
    return Ui::button(
        std::move(onPress),
        style,
        minSize(
            {72, 72},
            center(
                Ui::icon(i, 24))));
}

Ui::Child app() {
    auto content = Ui::reducer<Model>([](State state) {
        auto lbl = Ui::text(
            Ui::TextStyle::displayMedium(),
            "{}", state.counter);

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
                   Ui::grow(lbl),
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

} // namespace Counter

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Counter::app());
}

#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/button.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scafold.h>

#include "model.h"

namespace Counter {

Ui::Child rounedButton(Func<void(Ui::Node &)> onPress, Ui::ButtonStyle style, Media::Icons i) {
    return Ui::button(
        std::move(onPress),
        style,
        minSize(
            {72, 72},
            center(
                Ui::icon(i, 24))));
}

Ui::Child app() {
    auto content = Ui::reducer<Model>(0, reduce, [](auto state) {
        auto lbl = Ui::text(64, "{}", state);

        auto decBtn = Ui::button(Model::bind<DecrementAction>(), Ui::Button::DEFAULT_ROUND, Media::Icons::MINUS_THICK);
        auto incBtn = Ui::button(Model::bind<IncrementAction>(), Ui::Button::DEFAULT_ROUND, Media::Icons::PLUS_THICK);
        auto resetBtn = Ui::button(Model::bind<ResetAction>(), Ui::Button::SUBTLE_ROUND, Media::Icons::REFRESH, "RESET");

        return Ui::spacing(
            32,
            Ui::vflow(
                32,
                Layout::Align::CENTER,
                Ui::grow(lbl),
                Ui::hflow(
                    16,
                    decBtn,
                    incBtn),
                resetBtn));
    });

    return Ui::dialogLayer(
        Ui::minSize(
            420,
            Ui::vflow(
                Ui::titlebar(
                    Media::Icons::COUNTER,
                    "Counter",
                    Ui::TitlebarStyle::DIALOG),
                Ui::grow(content))));
}

} // namespace Counter

CliResult entryPoint(CliArgs args) {
    return Ui::runApp(args, Counter::app());
}

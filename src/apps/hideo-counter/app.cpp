#include <hideo-base/scafold.h>
#include <karm-ui/input.h>

#include "model.h"

namespace Hideo::Counter {

Ui::Child app() {
    return Ui::reducer<Model>([](State const &s) {
        return scafold({
            .icon = Mdi::COUNTER,
            .title = "Counter"s,
            .titlebar = Kr::TitlebarStyle::DIALOG,
            .body = [&] {
                auto decBtn = Ui::button(
                    Model::bind<DecrementAction>(),
                    Ui::ButtonStyle::regular().withRadii(999),
                    Mdi::MINUS_THICK
                );

                auto incBtn = Ui::button(
                    Model::bind<IncrementAction>(),
                    Ui::ButtonStyle::regular().withRadii(999),
                    Mdi::PLUS_THICK
                );

                auto resetBtn = Ui::button(
                    Model::bindIf<ResetAction>(not s.initial),
                    Ui::ButtonStyle::subtle().withRadii(999),
                    Mdi::REFRESH, "Reset"
                );

                return Ui::vflow(
                           32,
                           Math::Align::CENTER,
                           Ui::text(
                               Ui::TextStyles::codeLarge()
                                   .withSize(48),
                               "{}", s.counter
                           ) | Ui::grow(),
                           Ui::hflow(16, decBtn, incBtn),
                           resetBtn
                       ) |
                       Ui::insets(32);
            },
            .size = 420,
        });
    });
}

} // namespace Hideo::Counter

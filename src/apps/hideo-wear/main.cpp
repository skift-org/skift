#include <karm-math/align.h>
#include <karm-sys/entry.h>

import Karm.Ui;
import Karm.App;
import Karm.Image;
import Karm.Kira;
import Mdi;

using namespace Karm;

namespace Hideo::Wear {

enum struct Page {
    WATCH,
    APPS,
    NOTI
};

struct State {
    Page page = Page::WATCH;
    DateTime dateTime{
        .date = {1, Month::APRIL, 2024},
        .time = {56, 34, 12},
    };

    using Action = Union<Page>;

    Ui::Task<Action> reduce(Action const& action) {
        return action.visit(Visitor{
            [&](Page p) {
                page = p;
                return NONE;
            },
        });
    }
};

using Model = Ui::Model<State, State::Action>;

Ui::Child analogWatchface(DateTime dt) {
    auto [date, time] = dt;
    return Kr::clock(time);
}

Ui::Child digitalWatchface(DateTime dt) {
    auto [date, time] = dt;

    auto dateTime = Io::format(
        // Mon. 28 Jul
        "{}. {} {}",
        Io::toCapitalCase(date.dayOfWeek().abbr()),
        date.dayOfMonth() + 1,
        Io::toCapitalCase(date.month.str())
    );

    auto clock = Ui::vflow(
        16,
        Math::Align::CENTER,
        Ui::displayMedium("{02}:{02}", time.hour, time.minute),
        Ui::titleMedium(dateTime)
    );

    return clock |
           Ui::center();
}

Ui::Child menu() {
    return Ui::vflow(
               6,
               Ui::titleMedium("Applications") | Ui::center() | Ui::bound() | Ui::minSize({Ui::UNCONSTRAINED, 96}),
               Ui::button(Ui::SINK<>, Mdi::COG, "Settings"s),
               Ui::button(Ui::SINK<>, Mdi::COG, "Settings"s),
               Ui::button(Ui::SINK<>, Mdi::COG, "Settings"s),
               Ui::button(Ui::SINK<>, Mdi::COG, "Settings"s),
               Ui::button(Ui::SINK<>, Mdi::COG, "Settings"s),
               Ui::button(Ui::SINK<>, Mdi::COG, "Settings"s),
               Ui::button(Ui::SINK<>, Mdi::COG, "Settings"s)
           ) |
           Ui::insets(16) |
           Ui::vscroll();
}

Ui::Child app() {
    return Ui::reducer<Model>({}, [](State const& s) {
               return Ui::stack(
                          analogWatchface(s.dateTime),
                          menu() | Ui::box({.backgroundFill = Ui::GRAY900}) | Ui::slideIn(Ui::SlideFrom::END) | Ui::cond(s.page == Page::APPS)
                      ) |
                      Ui::keyboardShortcut(App::Key::M, Model::bind<Page>(Page::APPS)) |
                      Ui::keyboardShortcut(App::Key::ESC, Model::bind<Page>(Page::WATCH));
               ;
           }) |
           Ui::pinSize(192);
}

} // namespace Hideo::Wear

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(
        ctx,
        Hideo::Wear::app()
    );
}

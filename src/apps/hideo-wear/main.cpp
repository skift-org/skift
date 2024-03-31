#include <karm-sys/entry.h>
#include <karm-ui/app.h>

namespace Hideo::Wear {

Ui::Child app() {
    auto [date, time] = DateTime{};
    auto dateTime = Io::format(
        // Mon. 28 Jul
        "{}. {} {}",
        date.month.abbr(),
        date.dayOfMonth() + 1,
        date.year.val()
    );

    auto clock = Ui::vflow(
        16,
        Layout::Align::CENTER,
        Ui::displayMedium("{02}:{02}", time.hour, time.minute),
        Ui::titleMedium(dateTime.unwrap())
    );

    return Ui::stack(
               Ui::image(Media::loadImageOrFallback("bundle://skift-wallpapers/images/abstract.qoi"_url).unwrap()) | Ui::cover(),
               clock | Ui::center()
           ) |
           Ui::maxSize(192);
}

} // namespace Hideo::Wear

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(
        ctx,
        Hideo::Wear::app()
    );
}

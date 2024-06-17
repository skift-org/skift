#include <hideo-base/scafold.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/scroll.h>

namespace Hideo::Weather {

Ui::Child app() {
    return scafold({
        .icon = Mdi::WEATHER_PARTLY_CLOUDY,
        .title = "Weather"s,
        .body = slot$(Ui::image(Media::loadImage("bundle://hideo-weather/images/weather-few-clouds.jpg"_url).unwrap()) | Ui::cover()),
        .size = {460, 320},
    });
}

} // namespace Hideo::Weather

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    co_return Ui::runApp(ctx, Hideo::Weather::app());
}

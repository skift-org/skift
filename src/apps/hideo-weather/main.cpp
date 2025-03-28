#include <karm-image/loader.h>
#include <karm-kira/scaffold.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <mdi/weather-partly-cloudy.h>

namespace Hideo::Weather {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::WEATHER_PARTLY_CLOUDY,
        .title = "Weather"s,
        .body = [&] {
            auto image = Image::load("bundle://hideo-weather/images/weather-few-clouds.jpg"_url).unwrap();
            return Ui::image(image) | Ui::cover() | Ui::vhclip();
        },
    });
}

} // namespace Hideo::Weather

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(ctx, Hideo::Weather::app());
}

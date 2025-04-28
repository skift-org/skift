#include <karm-image/loader.h>
#include <karm-sys/entry.h>

import Mdi;
import Karm.Ui;
import Karm.Kira;

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

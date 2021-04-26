#include <libio/Format.h>
#include <libwidget/Button.h>

#include "panel/widgets/DateAndTime.h"

namespace panel
{

DateAndTime::DateAndTime()
{
    min_width(128);

    _timer = own<Async::Timer>(1000, [this]() {
        TimeStamp timestamp = timestamp_now();
        DateTime datetime = timestamp_to_datetime(timestamp);

        state(datetime);
    });

    _timer->start();
}

RefPtr<Widget::Element> DateAndTime::build()
{
    DateTime datetime = state();
    return Widget::button(Widget::Button::TEXT, IO::format("{02d}:{02d}:{02d}", datetime.hour, datetime.minute, datetime.second));
}

} // namespace panel

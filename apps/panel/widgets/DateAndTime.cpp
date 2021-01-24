#include <libwidget/Label.h>

#include "panel/widgets/DateAndTime.h"

namespace panel
{

DateAndTime::DateAndTime(Widget *parent) : Button(parent, Button::TEXT)
{
    auto label = new Label(this, "");

    _timer = own<Timer>(1000, [this, label]() {
        TimeStamp timestamp = timestamp_now();
        DateTime datetime = timestamp_to_datetime(timestamp);

        char buffer[256];
        snprintf(buffer, 256, "%02d:%02d:%02d ", datetime.hour, datetime.minute, datetime.second);

        label->text(buffer);
    });

    _timer->start();
}

} // namespace panel

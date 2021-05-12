#include "panel/widgets/DateAndTime.h"

using namespace Widget;

namespace Panel
{

RefPtr<Element> DateAndTimeComponent::build()
{
    return refresher(1000, [] {
        DateTime datetime = timestamp_to_datetime(timestamp_now());
        auto text = IO::format("{02d}:{02d}:{02d}", datetime.hour, datetime.minute, datetime.second);
        return basic_button(text);
    });
};

} // namespace Panel

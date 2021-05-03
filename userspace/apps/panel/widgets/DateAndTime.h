#pragma once

#include <libasync/Timer.h>

#include <libwidget/components/Statefull.h>

namespace panel
{

class DateAndTime : public Widget::Statefull<DateTime>
{
private:
    OwnPtr<Async::Timer> _timer;

public:
    DateAndTime();

    RefPtr<Widget::Element> build(DateTime state) override;
};

static inline RefPtr<DateAndTime> date_and_time(Callback<void(void)> on_click)
{
    auto date_and_time = make<DateAndTime>();

    if (on_click)
    {
        date_and_time->on(Widget::Event::ACTION, [on_click](auto) { on_click(); });
    }

    return date_and_time;
}

} // namespace panel

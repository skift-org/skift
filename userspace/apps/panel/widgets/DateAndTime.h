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

} // namespace panel

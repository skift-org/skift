#pragma once

#include <libasync/Timer.h>

#include <libwidget/Button.h>

namespace panel
{

class DateAndTime : public Widget::Button
{
private:
    OwnPtr<Async::Timer> _timer;

public:
    DateAndTime(Widget::Component *parent);
};

} // namespace panel

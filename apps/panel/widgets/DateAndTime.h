#pragma once

#include <libsystem/eventloop/Timer.h>

#include <libwidget/Button.h>

namespace panel
{

class DateAndTime : public Button
{
private:
    OwnPtr<Timer> _timer;

public:
    DateAndTime(Component *parent);
};

} // namespace panel

#pragma once

#include <libasync/Timer.h>

#include <libwidget/components/StatefullComponent.h>

namespace panel
{

class DateAndTime : public Widget::StatefullComponent<DateTime>
{
private:
    OwnPtr<Async::Timer> _timer;

public:
    DateAndTime();

    RefPtr<Widget::Element> build() override;
};

} // namespace panel

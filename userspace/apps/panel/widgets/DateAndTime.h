#pragma once

#include <libasync/Timer.h>
#include <libwidget/Components.h>

namespace Panel
{

struct DateAndTimeComponent :
    public Widget::Component
{
    Ref<Widget::Element> build() override;
};

WIDGET_BUILDER(DateAndTimeComponent, date_and_time);

} // namespace Panel

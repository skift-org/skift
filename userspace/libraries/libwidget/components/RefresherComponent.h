#pragma once

#include <libasync/Timer.h>
#include <libwidget/components/RebuildableComponent.h>

namespace Widget
{

struct Refresher : public RebuildableComponent
{
private:
    OwnPtr<Async::Timer> _timer;
    Func<RefPtr<Element>()> _callback;

public:
    Refresher(Timeout interval, Func<RefPtr<Element>()> callback)
        : _timer{own<Async::Timer>(interval, [this]() {
              should_rebuild();
          })},
          _callback{callback}
    {
        _timer->start();
    }

    RefPtr<Element> do_build() final
    {
        return _callback();
    }
};

WIDGET_BUILDER(Refresher, refresher);

} // namespace Widget

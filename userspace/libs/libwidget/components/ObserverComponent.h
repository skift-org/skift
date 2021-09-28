#pragma once

#include <libasync/Observable.h>
#include <libwidget/components/RebuildableComponent.h>

namespace Widget
{

template <typename TObservable>
struct ObserverComponent : public RebuildableComponent
{
private:
    OwnPtr<Async::Observer<TObservable>> _observer;
    Func<RefPtr<Element>()> _callback;

public:
    ObserverComponent(TObservable &observable, Func<RefPtr<Element>()> callback)
    {
        _observer = observable.observe([this](auto &) {
            should_rebuild();
        });

        _callback = callback;
    }

    RefPtr<Element> do_build() final
    {
        return _callback();
    }
};

template <typename TObservable>
static inline auto observer(TObservable &observable, Func<RefPtr<Element>()> callback) -> RefPtr<Element>
{
    return make<ObserverComponent<TObservable>>(observable, callback);
}

} // namespace Widget

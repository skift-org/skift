#pragma once

#include <libasync/Observable.h>
#include <libwidget/components/RebuildableComponent.h>

namespace Widget
{

template <typename TObservable, typename TCallback>
struct ObserverComponent : public RebuildableComponent
{
private:
    TCallback _callback;
    Async::Observer<TObservable> _observer;

public:
    ObserverComponent(TObservable observable)
    {
        _observer = observable.observe([this](auto &) {
            should_rebuild();
        });
    }

    RefPtr<Element> do_build() final
    {
        return _callback();
    }
};

template <typename TObservable, typename TCallback>
static inline RefPtr<ObserverComponent<TObservable, TCallback>> stateful(
    TObservable observable, TCallback callback)
{
    return make<ObserverComponent<TObservable, TCallback>>(observable, callback);
}

} // namespace Widget

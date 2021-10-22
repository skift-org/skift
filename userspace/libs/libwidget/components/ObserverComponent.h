#pragma once

#include <libasync/Observable.h>
#include <libwidget/components/RebuildableComponent.h>

namespace Widget
{

template <typename TObservable>
struct ObserverComponent : public RebuildableComponent
{
private:
    Box<Async::Observer<TObservable>> _observer;
    Func<Ref<Element>()> _callback;

public:
    ObserverComponent(TObservable &observable, Func<Ref<Element>()> callback)
    {
        _observer = observable.observe([this](auto &)
            { should_rebuild(); });

        _callback = callback;
    }

    Ref<Element> do_build() final
    {
        return _callback();
    }
};

template <typename TObservable>
static inline auto observer(TObservable &observable, Func<Ref<Element>()> callback) -> Ref<Element>
{
    return make<ObserverComponent<TObservable>>(observable, callback);
}

} // namespace Widget

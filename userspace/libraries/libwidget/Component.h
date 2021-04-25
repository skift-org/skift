#pragma once

#include <libasync/Invoker.h>
#include <libwidget/Element.h>

namespace Widget
{

class Component : public Element
{
private:
    OwnPtr<Async::Invoker> _rebuild_invoker;

public:
    void mounted() override { build(); }

    Component()
    {
        _rebuild_invoker = own<Async::Invoker>([&] {
            clear();
            build();
        });
    }

    void should_rebuild()
    {
        _rebuild_invoker->invoke_later();
    }

    virtual void build() {}
};

} // namespace Widget

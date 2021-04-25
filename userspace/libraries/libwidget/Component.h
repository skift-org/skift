#pragma once

#include <libasync/Invoker.h>
#include <libwidget/Element.h>
#include <libwidget/Layouts.h>

namespace Widget
{

class Component : public Element
{
private:
    OwnPtr<Async::Invoker> _rebuild_invoker;

public:
    void mounted() override { add(fill(build())); }

    Component()
    {
        _rebuild_invoker = own<Async::Invoker>([this] {
            clear();
            add(fill(build()));
        });
    }

    void should_rebuild()
    {
        _rebuild_invoker->invoke_later();
    }

    virtual RefPtr<Element> build() { return nullptr; }
};

} // namespace Widget

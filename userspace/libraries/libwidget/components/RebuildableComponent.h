#pragma once

#include <libasync/Invoker.h>
#include <libwidget/Layouts.h>
#include <libwidget/elements/PlaceholderElement.h>

namespace Widget
{

struct RebuildableComponent : public Element
{
private:
    OwnPtr<Async::Invoker> _rebuild_invoker;

public:
    void mounted() override
    {
        rebuild();
    }

    RebuildableComponent()
    {
        _rebuild_invoker = own<Async::Invoker>([this] {
            rebuild();
        });
    }

    void should_rebuild()
    {
        _rebuild_invoker->invoke_later();
    }

    void rebuild()
    {
        clear();
        add(fill(placeholder(do_build())));
    }

    virtual RefPtr<Element> do_build() { return nullptr; }
};

} // namespace Widget

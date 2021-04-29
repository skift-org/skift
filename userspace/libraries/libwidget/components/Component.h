#pragma once

#include <libasync/Invoker.h>
#include <libio/Streams.h>
#include <libwidget/Layouts.h>
#include <libwidget/elements/LabelElement.h>

namespace Widget
{

class Component : public Element
{
private:
    OwnPtr<Async::Invoker> _rebuild_invoker;

public:
    RefPtr<Element> check(RefPtr<Element> el)
    {
        if (el == nullptr)
        {
            IO::logln("Build function returned nullptr!");
            return label("Build function returned nullptr!");
        }
        else
        {
            return el;
        }
    }

    void mounted() override
    {
        add(fill(check(build())));
    }

    Component()
    {
        _rebuild_invoker = own<Async::Invoker>([this] {
            clear();
            add(fill(check(build())));
        });
    }

    void should_rebuild()
    {
        _rebuild_invoker->invoke_later();
    }

    virtual RefPtr<Element> build() { return nullptr; }
};

} // namespace Widget

#pragma once

#include <libwidget2/Diff.h>
#include <libwidget2/Transient.h>

namespace Widget2
{

template <typename TProps>
struct Rebuildable : public Element<TProps>
{
private:
    OwnPtr<Async::Invoker> _rebuild_invoker;
    RefPtr<BaseElement> _content;

public:
    Rebuildable()
    {
        _rebuild_invoker = own<Async::Invoker>([this] {
            rebuild();
        });
    }

    void should_rebuild()
    {
        _rebuild_invoker->invoke_later();
    }

    void rebuild() virtual
    {
        _content = diff(_content, do_build());
    }

    virtual OwnPtr<TransientBase> do_build() { return nullptr; }
};

} // namespace Widget2

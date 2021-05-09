#pragma once

#include <libwidget2/Rebuildable.h>

namespace Widget2
{

template <typename TProps>
struct Component : public Rebuildable<TProps>
{
    OwnPtr<BaseTransient> do_build() final { return build(props()); }

    virtual OwnPtr<BaseTransient> build(const TProps &)
    {
        return nullptr;
    }
};

} // namespace Widget2

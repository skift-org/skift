#pragma once

#include <libwidget/components/RebuildableComponent.h>

namespace Widget
{

struct Component : public RebuildableComponent
{
    Ref<Element> do_build() final { return build(); }
    virtual Ref<Element> build() { return nullptr; }
};

} // namespace Widget

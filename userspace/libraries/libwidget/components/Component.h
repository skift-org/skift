#pragma once

#include <libwidget/components/RebuildableComponent.h>

namespace Widget
{

struct Component : public RebuildableComponent
{
    RefPtr<Element> do_build() final { return build(); }
    virtual RefPtr<Element> build() { return nullptr; }
};

} // namespace Widget

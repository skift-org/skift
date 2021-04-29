#pragma once

#include <libwidget/components/Rebuildable.h>

namespace Widget
{

class Component : public Rebuildable
{
    RefPtr<Element> do_build() final { return build(); }
    virtual RefPtr<Element> build() { return nullptr; }
};

} // namespace Widget

#pragma once

#include <libwidget/Component.h>

namespace Widget
{

class Container : public Component
{
public:
    Container(Component *parent);
};

} // namespace Widget

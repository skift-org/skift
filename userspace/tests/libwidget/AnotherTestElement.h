#pragma once

#include <libwidget2/Element.h>

namespace Widget2::Tests
{

struct AnotherTestElementProps
{
    bool value = 0;

    auto operator<=>(const AnotherTestElementProps &) const = default;
};

struct AnotherTestElement : public Element<AnotherTestElementProps>
{
    AnotherTestElement(auto &props) : Element(props) {}
};

} // namespace Widget2::Test
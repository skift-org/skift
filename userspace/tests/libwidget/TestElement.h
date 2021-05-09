#include <libwidget2/Element.h>

namespace Widget2::Tests
{

struct TestElementProps
{
    int value = 0;

    auto operator<=>(const TestElementProps &) const = default;
};

struct TestElement : public Element<TestElementProps>
{
    TestElement(auto &props) : Element(props) {}
};

} // namespace Widget2::Test
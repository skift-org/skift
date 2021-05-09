#include <libwidget2/Diff.h>

#include "AnotherTestElement.h"
#include "TestElement.h"
#include "tests/Driver.h"

namespace Widget2::Tests
{

TEST(diff_props_change)
{
    auto el = make_element<TestElement>({42});
    auto tr = make_transient<TestElement>({52}, {});

    auto result = diff(el, tr);

    Assert::equal(el, result);
}

TEST(diff_type_change)
{
    auto el = make_element<TestElement>({42});
    auto tr = make_transient<AnotherTestElement>({true}, {});

    auto result = diff(el, tr);

    Assert::not_equal(el, result);
}

} // namespace Widget2::Tests
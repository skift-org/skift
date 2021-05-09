#include <libwidget2/Transient.h>

#include "TestElement.h"
#include "tests/Driver.h"

namespace Widget2::Tests
{

TEST(create_element_from_transiant)
{
    auto tr = make_transient<TestElement>({}, {});
    auto el = tr->create();
    Assert::not_null(dynamic_cast<TestElement *>(el.naked()));
}

TEST(inject_props_from_transiant)
{
    auto tr = make_transient<TestElement>({54}, {});
    auto el = tr->create();

    auto elptr = dynamic_cast<TestElement *>(el.naked());
    Assert::not_null(elptr);

    Assert::equal(elptr->props().value, 54);

    auto tr2 = make_transient<TestElement>({42}, {});
    tr2->inject(*el);

    Assert::equal(elptr->props().value, 42);
}

} // namespace Widget2
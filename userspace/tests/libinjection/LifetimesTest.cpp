#include <libinjection/Builder.h>

#include "tests/Driver.h"
#include "tests/libinjection/Asserts.h"
#include "tests/libinjection/mock/NumberAndString.h"

namespace Injection
{

TEST(container_fetch_transient)
{
    Container container;
    Builder builder{container};

    builder.transient<Type42>();
    builder.transient<Type42>();

    auto types = container.get_all<Type42>();

    Assert::equal(types.count(), 2);
    Assert::equal(types[0]->func(), 42);
    Assert::equal(types[1]->func(), 42);

    assert_not_same_entity(types[0], types[1]);
}

} // namespace Injection
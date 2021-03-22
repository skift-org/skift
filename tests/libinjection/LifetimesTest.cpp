#include <libinjection/Inject.h>

#include "tests/Driver.h"
#include "tests/libinjection/Asserts.h"
#include "tests/libinjection/mock/NumberAndString.h"

namespace Injection
{

TEST(container_fetch_transient)
{
    Container container;
    inject_transient<Type42>(container);
    inject_transient<Type42>(container);

    auto types = container.get_all<Type42>();

    assert_equal(types.count(), 2);
    assert_equal(types[0]->func(), 42);
    assert_equal(types[1]->func(), 42);

    assert_not_same_entity(types[0], types[1]);
}

} // namespace Injection
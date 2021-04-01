#include <libinjection/Inject.h>

#include "tests/Driver.h"
#include "tests/libinjection/Asserts.h"

namespace Injection
{

struct TestStruct : public Entity
{
    int fun() { return 1234; }
};

TEST(simple_constructor_factory)
{
    Container container;
    Injection::inject_transient<TestStruct>(container);

    auto fetched_instance = container.get<TestStruct>();
    Assert::not_null(fetched_instance);
    Assert::equal(fetched_instance->fun(), 1234);
}

TEST(constant_factory)
{
    Container container;

    auto original_instance = make<TestStruct>();
    Injection::inject_transient<TestStruct>(container, original_instance);

    auto fetched_instance = container.get<TestStruct>();

    assert_same_entity(original_instance, fetched_instance);
    Assert::equal(fetched_instance->fun(), 1234);
}

TEST(callback_factory)
{
    Container container;
    Injection::inject_transient<TestStruct>(container, [](Context &) { return make<TestStruct>(); });

    auto fetched_instance = container.get<TestStruct>();
    Assert::not_null(fetched_instance);
    Assert::equal(fetched_instance->fun(), 1234);
}

} // namespace Injection

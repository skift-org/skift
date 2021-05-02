#include <libinjection/Builder.h>

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
    Builder{container}
        .transient<TestStruct>();

    auto fetched_instance = container.get<TestStruct>();
    Assert::not_null(fetched_instance);
    Assert::equal(fetched_instance->fun(), 1234);
}

TEST(constant_factory)
{
    Container container;
    Builder builder{container};

    auto original_instance = make<TestStruct>();
    builder.transient<TestStruct>(original_instance);

    auto fetched_instance = container.get<TestStruct>();

    assert_same_entity(original_instance, fetched_instance);
    Assert::equal(fetched_instance->fun(), 1234);
}

TEST(callback_factory)
{
    Container container;
    Builder{container}
        .transient<TestStruct>([](Context &) { return make<TestStruct>(); });

    auto fetched_instance = container.get<TestStruct>();
    Assert::not_null(fetched_instance);
    Assert::equal(fetched_instance->fun(), 1234);
}

} // namespace Injection

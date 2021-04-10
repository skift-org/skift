#include <libinjection/Inject.h>

#include "tests/Driver.h"
#include "tests/libinjection/Asserts.h"

struct TestStruct : public Injection::Entity
{
    int fun() { return 1234; }
};

TEST(simple_constructor_factory)
{
    Injection::Container container;
    Injection::inject_transient<TestStruct>(container);

    auto fetched_instance = container.get<TestStruct>();
    Assert::not_null(fetched_instance);
    Assert::equal(fetched_instance->fun(), 1234);
}

TEST(constant_factory)
{
    Injection::Container container;

    auto original_instance = make<TestStruct>();
    Injection::inject_transient<TestStruct>(container, original_instance);

    auto fetched_instance = container.get<TestStruct>();

    assert_same_entity(original_instance, fetched_instance);
    Assert::equal(fetched_instance->fun(), 1234);
}

TEST(callback_factory)
{
    Injection::Container container;
    Injection::inject_transient<TestStruct>(container, [](Injection::Context &) { return make<TestStruct>(); });

    auto fetched_instance = container.get<TestStruct>();
    Assert::not_null(fetched_instance);
    Assert::equal(fetched_instance->fun(), 1234);
}

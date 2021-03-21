#include <libinjection/Container.h>
#include <libtest/Assets.h>

#include "tests/Driver.h"

#define assert_same_entity(__a, __b)                                          \
    assert_equal((uintptr_t) static_cast<Injection::Entity *>((__a).naked()), \
                 (uintptr_t) static_cast<Injection::Entity *>((__b).naked()))

#define assert_not_same_entity(__a, __b)                                          \
    assert_not_equal((uintptr_t) static_cast<Injection::Entity *>((__a).naked()), \
                     (uintptr_t) static_cast<Injection::Entity *>((__b).naked()))

struct NumericType :
    public virtual Injection::Entity
{
    virtual int func() = 0;
};

struct StringType :
    public virtual Injection::Entity
{
    virtual String string() = 0;
};

struct Type42 :
    public NumericType
{
    Type42(Injection::Context &) {}

    int func() override { return 42; }
};

struct Type52 :
    public NumericType
{
    Type52(Injection::Context &) {}

    int func() override { return 52; }
};

struct Type5Apple :
    public NumericType,
    public StringType
{
    Type5Apple(Injection::Context &) {}

    int func() override { return 5; }
    String string() { return "apple"; }
};

TEST(injection_container_fetch_simple_type)
{
    Injection::Container container;

    container.add_singleton<Type42>();
    auto instance = container.get<Type42>();

    assert_equal(instance->func(), 42);
}

TEST(injection_container_fetch_type_behind_interface)
{
    Injection::Container container;

    container.add_singleton<Type42, NumericType>();
    auto instance = container.get<NumericType>();

    assert_equal(instance->func(), 42);
}

TEST(injection_container_fetch_multiple_types)
{
    Injection::Container container;

    container.add_singleton<Type42, NumericType>();
    container.add_singleton<Type52, NumericType>();

    auto numeric_types = container.get_all<NumericType>();

    assert_equal(numeric_types.count(), 2);
    assert_equal(numeric_types[0]->func(), 42);
    assert_equal(numeric_types[1]->func(), 52);
}

TEST(injection_container_fetch_type_behind_multiple_interfaces)
{
    Injection::Container container;
    container.add_singleton<Type5Apple, NumericType, StringType>();

    auto numeric_type = container.get<NumericType>();
    auto string_type = container.get<StringType>();

    assert_not_null(numeric_type);
    assert_not_null(string_type);

    // Is it the same undelying Injection::Entity ?
    assert_same_entity(numeric_type, string_type);

    assert_equal(numeric_type->func(), 5);
    assert_equal(string_type->string(), "apple");
}

TEST(injection_container_fetch_transient)
{
    Injection::Container container;

    container.add_transient<Type42>();
    container.add_transient<Type42>();

    auto types = container.get_all<Type42>();

    assert_equal(types.count(), 2);
    assert_equal(types[0]->func(), 42);
    assert_equal(types[1]->func(), 42);

    assert_not_same_entity(types[0], types[1]);
}

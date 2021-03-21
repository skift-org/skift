#include <libioc/Container.h>
#include <libtest/Assets.h>

#include "tests/Driver.h"

struct NumericType :
    public virtual IOC::Entity
{
    virtual int func() = 0;
};

struct StringType :
    public virtual IOC::Entity
{
    virtual String string() = 0;
};

struct Type42 :
    public NumericType
{
    Type42(IOC::Context &) {}

    int func() override { return 42; }
};

struct Type52 :
    public NumericType
{
    Type52(IOC::Context &) {}

    int func() override { return 52; }
};

struct Type5Apple :
    public NumericType,
    public StringType
{
    Type5Apple(IOC::Context &) {}

    int func() override { return 5; }
    String string() { return "apple"; }
};

TEST(ioc_container_fetch_simple_type)
{
    IOC::Container container;

    container.add_singleton<Type42>();
    auto instance = container.get<Type42>();

    assert_equal(instance->func(), 42);
}

TEST(ioc_container_fetch_simple)
{
    IOC::Container container;

    container.add_singleton<Type42>();
    auto instance = container.get<Type42>();

    assert_equal(instance->func(), 42);
}

TEST(ioc_container_fetch_type_behind_interface)
{
    IOC::Container container;

    container.add_singleton<Type42, NumericType>();
    auto instance = container.get<NumericType>();

    assert_equal(instance->func(), 42);
}

TEST(ioc_container_fetch_multiple_types)
{
    IOC::Container container;

    container.add_singleton<Type42, NumericType>();
    container.add_singleton<Type52, NumericType>();

    Vector<RefPtr<NumericType>> numeric_types;
    container.get(numeric_types);

    assert_equal(numeric_types.count(), 2);
    assert_equal(numeric_types[0]->func(), 42);
    assert_equal(numeric_types[1]->func(), 52);
}

TEST(ioc_container_fetch_type_behind_multiple_interfaces)
{
    IOC::Container container;
    container.add_singleton<Type5Apple, NumericType, StringType>();

    auto numeric_type = container.get<NumericType>();
    auto string_type = container.get<StringType>();

    assert_not_null(numeric_type);
    assert_not_null(string_type);

    // Is it the same undelying IOC::Entity ?
    assert_equal((uintptr_t) static_cast<IOC::Entity *>(numeric_type.naked()),
                 (uintptr_t) static_cast<IOC::Entity *>(string_type.naked()));

    assert_equal(numeric_type->func(), 5);
    assert_equal(string_type->string(), "apple");
}

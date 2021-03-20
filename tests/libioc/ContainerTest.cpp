#include <libioc/Container.h>
#include <libtest/Assets.h>

#include "tests/Driver.h"

struct NumericType : public RefCounted<NumericType>
{
    virtual ~NumericType() = default;
    virtual int func() = 0;
};

struct Type42 : public NumericType
{
    Type42(IOC::Context &) {}

    int func() override { return 42; }
};

struct Type52 : public NumericType
{
    Type52(IOC::Context &) {}

    int func() override { return 52; }
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

    container.add_singleton<NumericType, Type42>();
    auto instance = container.get<NumericType>();

    assert_equal(instance->func(), 42);
}

TEST(ioc_container_fetch_multiple_types)
{
    IOC::Container container;

    container.add_singleton<NumericType, Type42>();
    container.add_singleton<NumericType, Type52>();

    Vector<RefPtr<NumericType>> numeric_types;
    container.get(numeric_types);

    assert_equal(numeric_types.count(), 2);

    assert_equal(numeric_types[0]->func(), 42);
    assert_equal(numeric_types[1]->func(), 52);
}

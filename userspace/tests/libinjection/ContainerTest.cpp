#include <libinjection/Builder.h>

#include "tests/Driver.h"
#include "tests/libinjection/Asserts.h"
#include "tests/libinjection/mock/Car.h"
#include "tests/libinjection/mock/NumberAndString.h"
#include "tests/libinjection/mock/Zoo.h"

namespace Injection
{

TEST(container_fetch_simple_type)
{
    Container container;
    Builder{container}
        .singleton<Type42>();

    auto instance = container.get<Type42>();

    Assert::equal(instance->func(), 42);
}

TEST(container_fetch_type_behind_interface)
{
    Container container;
    Builder{container}
        .singleton<Type42, NumericType>();

    auto instance = container.get<NumericType>();

    Assert::equal(instance->func(), 42);
}

TEST(container_fetch_multiple_types)
{
    Container container;
    Builder{container}
        .singleton<Type42, NumericType>()
        .singleton<Type52, NumericType>();

    auto numeric_types = container.get_all<NumericType>();

    Assert::equal(numeric_types.count(), 2);
    Assert::equal(numeric_types[0]->func(), 42);
    Assert::equal(numeric_types[1]->func(), 52);
}

TEST(container_fetch_type_behind_multiple_interfaces)
{
    Container container;
    Builder{container}
        .singleton<Type5Apple, NumericType, StringType>();

    auto numeric_type = container.get<NumericType>();
    auto string_type = container.get<StringType>();

    Assert::not_null(numeric_type);
    Assert::not_null(string_type);

    // Is it the same undelying Entity ?
    assert_same_entity(numeric_type, string_type);

    Assert::equal(numeric_type->func(), 5);
    Assert::equal(string_type->string(), "apple");
}

TEST(constructor_injection)
{
    Container container;
    Builder{container}
        .singleton<Car>()
        .singleton<Engine>();

    auto car = container.get<Car>();
    auto engine = container.get<Engine>();

    Assert::not_null(car);
    Assert::not_null(engine);

    Assert::equal(car->vroom(), 42);
    assert_same_entity(car->_engine, engine);
}

TEST(constructor_injection_list)
{
    Container container;
    Builder{container}
        .singleton<Lion, Animal>()
        .singleton<Cat, Animal>()
        .singleton<Dog, Animal>()
        .singleton<Zoo>();

    auto zoo = container.get<Zoo>();

    Assert::not_null(zoo);
    Assert::equal(zoo->_animals.count(), 3);
}

} // namespace Injection

#include <libinjection/Inject.h>
#include <libtest/Asserts.h>

#include "tests/Driver.h"
#include "tests/libinjection/Asserts.h"
#include "tests/libinjection/mock/Car.h"
#include "tests/libinjection/mock/NumberAndString.h"
#include "tests/libinjection/mock/Zoo.h"

TEST(injection_container_fetch_simple_type)
{
    Injection::Container container;
    Injection::inject_singleton<Type42>(container);

    auto instance = container.get<Type42>();

    assert_equal(instance->func(), 42);
}

TEST(injection_container_fetch_type_behind_interface)
{
    Injection::Container container;
    Injection::inject_singleton<Type42, NumericType>(container);

    auto instance = container.get<NumericType>();

    assert_equal(instance->func(), 42);
}

TEST(injection_container_fetch_multiple_types)
{
    Injection::Container container;
    Injection::inject_singleton<Type42, NumericType>(container);
    Injection::inject_singleton<Type52, NumericType>(container);

    auto numeric_types = container.get_all<NumericType>();

    assert_equal(numeric_types.count(), 2);
    assert_equal(numeric_types[0]->func(), 42);
    assert_equal(numeric_types[1]->func(), 52);
}

TEST(injection_container_fetch_type_behind_multiple_interfaces)
{
    Injection::Container container;
    Injection::inject_singleton<Type5Apple, NumericType, StringType>(container);

    auto numeric_type = container.get<NumericType>();
    auto string_type = container.get<StringType>();

    assert_not_null(numeric_type);
    assert_not_null(string_type);

    // Is it the same undelying Injection::Entity ?
    assert_same_entity(numeric_type, string_type);

    assert_equal(numeric_type->func(), 5);
    assert_equal(string_type->string(), "apple");
}

TEST(injection_constructor_injection)
{
    Injection::Container container;
    Injection::inject_singleton<Car>(container);
    Injection::inject_singleton<Engine>(container);

    auto car = container.get<Car>();
    auto engine = container.get<Engine>();

    assert_not_null(car);
    assert_not_null(engine);

    assert_equal(car->vroom(), 42);
    assert_same_entity(car->_engine, engine);
}

TEST(injection_constructor_injection_list)
{
    Injection::Container container;

    Injection::inject_singleton<Lion, Animal>(container);
    Injection::inject_singleton<Cat, Animal>(container);
    Injection::inject_singleton<Dog, Animal>(container);
    Injection::inject_singleton<Zoo>(container);

    auto zoo = container.get<Zoo>();

    assert_not_null(zoo);
    assert_equal(zoo->_animals.count(), 3);
}

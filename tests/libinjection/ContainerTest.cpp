#include <libinjection/Inject.h>
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

struct Engine : public Injection::Entity
{
    int vroom()
    {
        return 42;
    }
};

struct Car : public Injection::Entity
{
    RefPtr<Engine> _engine;

    INJECTOR(Car(RefPtr<Engine> engine))
        : _engine{engine}
    {
    }

    int vroom()
    {
        return _engine->vroom();
    }
};

struct Animal : public Injection::Entity
{
    virtual String speak() = 0;
};

struct Lion : public Animal
{
    virtual String speak() { return "Rhaaaa"; }
};

struct Cat : public Animal
{
    virtual String speak() { return "Maiou"; }
};

struct Dog : public Animal
{
    virtual String speak() { return "Bark"; }
};

struct Zoo : public Injection::Entity
{
    Vector<RefPtr<Animal>> _animals;

    INJECTOR(Zoo(Vector<RefPtr<Animal>> animal))
        : _animals{animal}
    {
    }
};

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

TEST(injection_container_fetch_transient)
{
    Injection::Container container;
    Injection::inject_transient<Type42>(container);
    Injection::inject_transient<Type42>(container);

    auto types = container.get_all<Type42>();

    assert_equal(types.count(), 2);
    assert_equal(types[0]->func(), 42);
    assert_equal(types[1]->func(), 42);

    assert_not_same_entity(types[0], types[1]);
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
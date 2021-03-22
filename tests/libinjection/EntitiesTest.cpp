#include <libinjection/Entity.h>

struct EntityWithSimpleConstructor
{
    EntityWithSimpleConstructor() {}
};

static_assert(!Injection::ContextInjectable<EntityWithSimpleConstructor>);
static_assert(!Injection::ConstructorInjectable<EntityWithSimpleConstructor>);

struct EntityWithContextInjection
{
    EntityWithContextInjection(Injection::Context &) {}
};

static_assert(Injection::ContextInjectable<EntityWithContextInjection>);
static_assert(!Injection::ConstructorInjectable<EntityWithContextInjection>);

struct EntityWithConstructorInjection
{
    INJECTOR(EntityWithConstructorInjection()) {}
};

static_assert(!Injection::ContextInjectable<EntityWithConstructorInjection>);
static_assert(Injection::ConstructorInjectable<EntityWithConstructorInjection>);

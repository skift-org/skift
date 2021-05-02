#include <libinjection/Entity.h>

namespace Injection
{

struct EntityWithSimpleConstructor
{
    EntityWithSimpleConstructor() {}
};

static_assert(!ContextInjectable<EntityWithSimpleConstructor>);
static_assert(!ConstructorInjectable<EntityWithSimpleConstructor>);

struct EntityWithContextInjection
{
    EntityWithContextInjection(Context &) {}
};

static_assert(ContextInjectable<EntityWithContextInjection>);
static_assert(!ConstructorInjectable<EntityWithContextInjection>);

struct EntityWithConstructorInjection
{
    INJECTOR(EntityWithConstructorInjection()) {}
};

static_assert(!ContextInjectable<EntityWithConstructorInjection>);
static_assert(ConstructorInjectable<EntityWithConstructorInjection>);

} // namespace Injection
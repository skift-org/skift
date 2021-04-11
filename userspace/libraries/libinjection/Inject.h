#pragma once

#include <libinjection/Container.h>
#include <libinjection/Factories.h>

namespace Injection
{

template <typename TInstance>
void inject_singleton(Container &container)
{
    inject_singleton<TInstance, TInstance>(container);
}

template <typename TInstance, typename... TInterfaces>
requires(sizeof...(TInterfaces) > 0) void inject_singleton(Container &container)
{
    ConstructorFactory<TInstance> factory;
    container.add<ConstructorFactory, SingletonLifeTime, TInstance, TInterfaces...>(factory);
}

template <typename TInstance>
void inject_singleton(Container &container, RefPtr<TInstance> instance)
{
    inject_singleton<TInstance, TInstance>(container, instance);
}

template <typename TInstance, typename... TInterfaces>
requires(sizeof...(TInterfaces) > 0) void inject_singleton(Container &container, RefPtr<TInstance> instance)
{
    ConstantFactory<TInstance> factory{instance};
    container.add<ConstantFactory, SingletonLifeTime, TInstance, TInterfaces...>(factory);
}

template <typename TInstance>
void inject_singleton(Container &container, Callback<RefPtr<TInstance>()> instance)
{
    inject_singleton<TInstance, TInstance>(container, instance);
}

template <typename TInstance, typename... TInterfaces>
requires(sizeof...(TInterfaces) > 0) void inject_singleton(Container &container, Callback<RefPtr<TInstance>()> instance)
{
    CallbackFactory<TInstance> factory{instance};
    container.add<CallbackFactory, SingletonLifeTime, TInstance, TInterfaces...>(factory);
}

template <typename TInstance>
void inject_transient(Container &container) { inject_transient<TInstance, TInstance>(container); }

template <typename TInstance, typename... TInterfaces>
requires(sizeof...(TInterfaces) > 0) void inject_transient(Container &container)
{
    ConstructorFactory<TInstance> factory;
    container.add<ConstructorFactory, TransientLifeTime, TInstance, TInterfaces...>(factory);
}

template <typename TInstance>
void inject_transient(Container &container, RefPtr<TInstance> instance)
{
    inject_transient<TInstance, TInstance>(container, instance);
}

template <typename TInstance, typename... TInterfaces>
requires(sizeof...(TInterfaces) > 0) void inject_transient(Container &container, RefPtr<TInstance> instance)
{
    ConstantFactory<TInstance> factory{instance};
    container.add<ConstantFactory, TransientLifeTime, TInstance, TInterfaces...>(factory);
}

template <typename TInstance>
void inject_transient(Container &container, Callback<RefPtr<TInstance>(Context &)> instance)
{
    inject_transient<TInstance, TInstance>(container, instance);
}

template <typename TInstance, typename... TInterfaces>
requires(sizeof...(TInterfaces) > 0) void inject_transient(Container &container, Callback<RefPtr<TInstance>(Context &)> instance)
{
    CallbackFactory<TInstance> factory{instance};
    container.add<CallbackFactory, TransientLifeTime, TInstance, TInterfaces...>(factory);
}

} // namespace Injection

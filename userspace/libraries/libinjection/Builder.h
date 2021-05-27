#pragma once

#include <libinjection/Container.h>
#include <libinjection/Factories.h>

namespace Injection
{

struct Builder
{
private:
    Container &_container;

    NONCOPYABLE(Builder);
    NONMOVABLE(Builder);

public:
    Builder(Container &container) : _container{container}
    {
    }

    template <typename TInstance>
    Builder &singleton()
    {
        return singleton<TInstance, TInstance>();
    }

    template <typename TInstance, typename... TInterfaces>
    requires(sizeof...(TInterfaces) > 0) Builder &singleton()
    {
        ConstructorFactory<TInstance> factory;
        _container.add<ConstructorFactory, SingletonLifeTime, TInstance, TInterfaces...>(factory);
        return *this;
    }

    template <typename TInstance>
    Builder &singleton(RefPtr<TInstance> instance)
    {
        return singleton<TInstance, TInstance>(instance);
    }

    template <typename TInstance, typename... TInterfaces>
    requires(sizeof...(TInterfaces) > 0) Builder &singleton(RefPtr<TInstance> instance)
    {
        ConstantFactory<TInstance> factory{instance};
        _container.add<ConstantFactory, SingletonLifeTime, TInstance, TInterfaces...>(factory);
        return *this;
    }

    template <typename TInstance>
    Builder &singleton(Func<RefPtr<TInstance>()> instance)
    {
        return singleton<TInstance, TInstance>(instance);
    }

    template <typename TInstance, typename... TInterfaces>
    requires(sizeof...(TInterfaces) > 0) Builder &singleton(Func<RefPtr<TInstance>()> instance)
    {
        CallbackFactory<TInstance> factory{instance};
        _container.add<CallbackFactory, SingletonLifeTime, TInstance, TInterfaces...>(factory);
        return *this;
    }

    template <typename TInstance>
    Builder &transient()
    {
        return transient<TInstance, TInstance>();
    }

    template <typename TInstance, typename... TInterfaces>
    requires(sizeof...(TInterfaces) > 0) Builder &transient()
    {
        ConstructorFactory<TInstance> factory;
        _container.add<ConstructorFactory, TransientLifeTime, TInstance, TInterfaces...>(factory);
        return *this;
    }

    template <typename TInstance>
    Builder &transient(RefPtr<TInstance> instance)
    {
        return transient<TInstance, TInstance>(instance);
    }

    template <typename TInstance, typename... TInterfaces>
    requires(sizeof...(TInterfaces) > 0) Builder &transient(RefPtr<TInstance> instance)
    {
        ConstantFactory<TInstance> factory{instance};
        _container.add<ConstantFactory, TransientLifeTime, TInstance, TInterfaces...>(factory);
        return *this;
    }

    template <typename TInstance>
    Builder &transient(Func<RefPtr<TInstance>(Context &)> instance)
    {
        return transient<TInstance, TInstance>(instance);
    }

    template <typename TInstance, typename... TInterfaces>
    requires(sizeof...(TInterfaces) > 0) Builder &transient(Func<RefPtr<TInstance>(Context &)> instance)
    {
        CallbackFactory<TInstance> factory{instance};
        _container.add<CallbackFactory, TransientLifeTime, TInstance, TInterfaces...>(factory);
        return *this;
    }
};

} // namespace Injection

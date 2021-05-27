#pragma once

#include <libinjection/Container.h>
#include <libinjection/Context.h>
#include <libinjection/Entity.h>

namespace Injection
{

template <typename T>
struct ConstantFactory
{
private:
    RefPtr<T> _instance;

public:
    ConstantFactory(RefPtr<T> instance) : _instance(instance) {}

    RefPtr<T> instance(Context &)
    {
        return _instance;
    }
};

template <typename TInstance>
struct ConstructorInvoker;

template <typename TInstance, typename... TConstructorArgs>
struct ConstructorInvoker<TInstance(TConstructorArgs...)>
{
    static RefPtr<TInstance> invoke(Context &context)
    {
        Container &container = context.container();

        return make<TInstance>(container.query<TConstructorArgs>(context)...);
    }
};

template <typename T>
struct ConstructorFactory
{
    RefPtr<T> instance(Context &context)
    {
        if constexpr (ContextInjectable<T>)
        {
            return make<T>(context);
        }
        else if constexpr (ConstructorInjectable<T>)
        {
            return ConstructorInvoker<typename T::__INJECTOR__>::invoke(context);
        }
        else
        {
            return make<T>();
        }
    }
};

template <typename T>
struct CallbackFactory
{
private:
    Func<RefPtr<T>(Context &)> _callback;

public:
    CallbackFactory(Func<RefPtr<T>(Context &)> callback)
        : _callback{callback}
    {
    }

    RefPtr<T> instance(Context &context)
    {
        return _callback(context);
    }
};

} // namespace Injection

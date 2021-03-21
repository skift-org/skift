#pragma once

#include <libinjection/Context.h>

namespace Injection
{

template <typename T>
struct ConstantFactory
{
private:
    RefPtr<T> _instance;

public:
    ConstantFactory(RefPtr<T> instance) : _instance(instance)
    {
    }

    RefPtr<T> instance(Context &)
    {
        return _instance;
    }
};

template <typename T>
struct ConstructorFactory
{
    RefPtr<T> instance(Context &context)
    {
        return make<T>(context);
    }
};

template <typename T>
struct CallbackFactory
{
private:
    Callback<T()> _callback;

public:
    RefPtr<T> instance(Context &context)
    {
        return _callback(context);
    }
};

} // namespace Injection

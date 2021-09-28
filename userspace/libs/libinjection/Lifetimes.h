#pragma once

#include <libinjection/Context.h>

namespace Injection
{

template <typename TInstance, typename TFactory>
struct TransientLifeTime : public RefCounted<TransientLifeTime<TInstance, TFactory>>
{
private:
    TFactory _factorie;

public:
    TransientLifeTime(TFactory factorie)
        : _factorie{factorie}
    {
    }

    virtual RefPtr<TInstance> instance(Context &context)
    {
        context.push_type<TInstance>();

        context.ensure_no_cycle();

        auto instance = _factorie.instance(context);

        context.pop_type<TInstance>();

        return instance;
    }
};

template <typename TInstance, typename TFactory>
struct SingletonLifeTime : public RefCounted<SingletonLifeTime<TInstance, TFactory>>
{
private:
    TFactory _factorie;
    RefPtr<TInstance> _instance;

public:
    SingletonLifeTime(TFactory factorie)
        : _factorie{factorie}
    {
    }

    virtual RefPtr<TInstance> instance(Context &context)
    {
        context.push_type<TInstance>();

        context.ensure_no_cycle();

        if (_instance == nullptr)
        {
            _instance = _factorie.instance(context);
        }

        context.pop_type<TInstance>();

        return _instance;
    }
};

} // namespace Injection

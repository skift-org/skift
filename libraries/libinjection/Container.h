#pragma once

#include <libtest/Assets.h>

#include <libutils/Callback.h>
#include <libutils/HashMap.h>
#include <libutils/RefPtr.h>
#include <libutils/String.h>
#include <libutils/Traits.h>
#include <libutils/TypeId.h>
#include <libutils/Vector.h>

namespace Injection
{

struct Container;

struct Entity : public AnyRef
{
};

struct Context
{
private:
    Vector<TypeId> _stack;
    Container &_container;

public:
    Container &container() const { return _container; }

    Context(Container &container)
        : _container{container}
    {
    }

    template <typename T>
    void push_type()
    {
        _stack.push_back(GetTypeId<T>());
    }

    void ensure_no_cycle()
    {
        for (size_t i = 0; i < _stack.count() - 1; ++i)
        {
            assert_not_equal(_stack[i], _stack.peek_back());
        }
    }

    template <typename T>
    void pop_type()
    {
        assert_equal(_stack.pop_back(), GetTypeId<T>());
    }
};

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

template <typename TInterface>
struct Retriever : public AnyRef
{
    virtual RefPtr<TInterface> instance(Context &context) = 0;
};

template <typename TInterface, typename TInstance>
struct CastRetriever : public Retriever<TInterface>
{
private:
    RefPtr<TInstance> _storage;

public:
    CastRetriever(RefPtr<TInstance> storage) : _storage(storage)
    {
    }

    RefPtr<TInterface> instance(Context &context) override
    {
        return _storage->instance(context);
    }
};

struct Container
{
private:
    HashMap<TypeId, Vector<RefPtr<AnyRef>>> _retrievers;

public:
    Container() {}

    ~Container() {}

    template <typename TLifetime, typename... TInterfaces>
    requires(sizeof...(TInterfaces) == 0) void register_interfaces(RefPtr<TLifetime>) {}

    template <typename TLifetime, typename TInterface, typename... TInterfaces>
    void register_interfaces(RefPtr<TLifetime> lifetime)
    {
        auto cast = make<CastRetriever<TInterface, TLifetime>>(lifetime);
        _retrievers[GetTypeId<TInterface>()].push_back(cast);

        register_interfaces<TLifetime, TInterfaces...>(lifetime);
    }

    template <typename TInstance>
    Container &add_singleton() { return add_singleton<TInstance, TInstance>(); }

    template <typename TInstance, typename... TInterfaces>
    requires(sizeof...(TInterfaces) > 0) Container &add_singleton()
    {
        ConstructorFactory<TInstance> factory;

        using LifetimeType = SingletonLifeTime<TInstance, ConstructorFactory<TInstance>>;

        auto lifetime = make<LifetimeType>(factory);

        register_interfaces<LifetimeType, TInterfaces...>(lifetime);

        return *this;
    }

    template <typename TInstance>
    Container &add_transient() { return add_transient<TInstance, TInstance>(); }

    template <typename TInstance, typename... TInterfaces>
    requires(sizeof...(TInterfaces) > 0) Container &add_transient()
    {
        ConstructorFactory<TInstance> factory;

        using LifetimeType = TransientLifeTime<TInstance, ConstructorFactory<TInstance>>;

        auto lifetime = make<LifetimeType>(factory);

        register_interfaces<LifetimeType, TInterfaces...>(lifetime);

        return *this;
    }

    template <typename TInterface>
    RefPtr<TInterface> get()
    {
        auto id = GetTypeId<TInterface>();

        if (!_retrievers.has_key(id))
        {
            return nullptr;
        }

        RefPtr<AnyRef> retriever = _retrievers[id].peek();

        Context context{*this};
        return static_cast<RefPtr<Retriever<TInterface>>>(retriever)->instance(context);
    }

    template <typename TInterface>
    void get(Vector<RefPtr<TInterface>> &instances)
    {
        auto id = GetTypeId<TInterface>();

        if (!_retrievers.has_key(id))
        {
            return;
        }

        Vector<RefPtr<AnyRef>> &retrievers = _retrievers[id];

        for (size_t i = 0; i < retrievers.count(); i++)
        {
            Context context{*this};
            instances.push_back(static_cast<RefPtr<Retriever<TInterface>>>(retrievers[i])->instance(context));
        }
    }
};

} // namespace Injection

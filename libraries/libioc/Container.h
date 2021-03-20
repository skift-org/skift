#pragma once

#include <libtest/Assets.h>
#include <libutils/Callback.h>
#include <libutils/HashMap.h>
#include <libutils/RefPtr.h>
#include <libutils/String.h>
#include <libutils/Traits.h>
#include <libutils/TypeId.h>
#include <libutils/Vector.h>

namespace IOC
{

struct Context
{
};

struct AnyRef : public RefCounted<AnyRef>
{
    virtual ~AnyRef() = default;
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
struct TransientInstance : public RefCounted<TransientInstance<TInstance, TFactory>>
{
private:
    TFactory _factorie;

public:
    TransientInstance(TFactory factorie) : _factorie{factorie}
    {
    }

    virtual RefPtr<TInstance> instance(Context &context)
    {
        return _factorie.instance(context);
    }
};

template <typename TInstance, typename TFactory>
struct SingletonInstance : public RefCounted<SingletonInstance<TInstance, TFactory>>
{
private:
    TFactory _factorie;
    RefPtr<TInstance> _instance;

public:
    SingletonInstance(TFactory factorie) : _factorie{factorie}
    {
    }

    virtual RefPtr<TInstance> instance(Context &context)
    {
        if (_instance == nullptr)
        {
            _instance = _factorie.instance(context);
        }

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

    template <typename TInterface, typename TInstance>
    Container &add_singleton()
    {
        ConstructorFactory<TInstance> factory;

        using InstanceType = SingletonInstance<TInstance, ConstructorFactory<TInstance>>;

        auto instance = make<InstanceType>(factory);

        auto cast = make<CastRetriever<TInterface, InstanceType>>(instance);

        _retrievers[GetTypeId<TInterface>()].push_back(cast);

        return *this;
    }

    template <typename TInstance>
    Container &add_singleton()
    {
        return add_singleton<TInstance, TInstance>();
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

        Context ctx;
        return static_cast<RefPtr<Retriever<TInterface>>>(retriever)->instance(ctx);
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
            Context ctx;
            instances.push_back(static_cast<RefPtr<Retriever<TInterface>>>(retrievers[i])->instance(ctx));
        }
    }
};

} // namespace IOC
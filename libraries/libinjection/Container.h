#pragma once

#include <libtest/Assets.h>

#include <libutils/HashMap.h>
#include <libutils/TypeId.h>
#include <libutils/Vector.h>

#include <libinjection/Entity.h>
#include <libinjection/Factories.h>
#include <libinjection/Lifetimes.h>
#include <libinjection/Retriever.h>

namespace Injection
{

struct Container
{
private:
    HashMap<TypeId, Vector<RefPtr<AnyRef>>> _retrievers;

public:
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
    requires(!IsVector<TInterface>::value) RefPtr<TInterface> get()
    {
        TypeId id = GetTypeId<TInterface>();

        if (!_retrievers.has_key(id))
        {
            return nullptr;
        }

        RefPtr<AnyRef> retriever = _retrievers[id].peek();

        Context context{*this};
        return static_cast<RefPtr<Retriever<TInterface>>>(retriever)->instance(context);
    }

    template <typename TInterface>
    Vector<RefPtr<TInterface>> get_all()
    {
        Vector<RefPtr<TInterface>> instances;

        TypeId id = GetTypeId<TInterface>();

        if (!_retrievers.has_key(id))
        {
            return instances;
        }

        Vector<RefPtr<AnyRef>> &retrievers = _retrievers[id];

        for (size_t i = 0; i < retrievers.count(); i++)
        {
            Context context{*this};
            instances.push_back(static_cast<RefPtr<Retriever<TInterface>>>(retrievers[i])->instance(context));
        }

        return instances;
    }
};

} // namespace Injection

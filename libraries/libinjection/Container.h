#pragma once

#include <libtest/Asserts.h>

#include <libutils/HashMap.h>
#include <libutils/TypeId.h>
#include <libutils/Vector.h>

#include <libinjection/Entity.h>
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

    template <
        template <typename> typename TFactory,
        template <typename, typename> typename TLifetime,
        typename TInstance,
        typename... TInterfaces>
    Container &add(TFactory<TInstance> factory)
    {
        using LifetimeType = TLifetime<TInstance, TFactory<TInstance>>;

        auto lifetime = make<LifetimeType>(factory);

        register_interfaces<LifetimeType, TInterfaces...>(lifetime);

        return *this;
    }

    template <typename TInterface>
    RefPtr<TInterface> get(Context &context)
    {
        TypeId id = GetTypeId<TInterface>();

        if (!_retrievers.has_key(id))
        {
            return nullptr;
        }

        RefPtr<AnyRef> retriever = _retrievers[id].peek();

        return static_cast<RefPtr<Retriever<TInterface>>>(retriever)->instance(context);
    }

    template <typename TInterface>
    RefPtr<TInterface> get()
    {
        Context context{*this};
        return get<TInterface>(context);
    }

    template <typename TInterface>
    Vector<RefPtr<TInterface>> get_all(Context &context)
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
            instances.push_back(static_cast<RefPtr<Retriever<TInterface>>>(retrievers[i])->instance(context));
        }

        return instances;
    }

    template <typename TInterface>
    Vector<RefPtr<TInterface>> get_all()
    {
        Context context{*this};
        return get_all<TInterface>(context);
    }

    template <typename TQuery>
    requires(!IsVector<TQuery>::value) TQuery query(Context &context)
    {
        using TInterface = typename TrimRefPtr<TQuery>::type;
        return get<TInterface>(context);
    }

    template <typename TQuery>
    requires(IsVector<TQuery>::value) TQuery query(Context &context)
    {
        using TInterface = typename TrimRefPtr<typename TrimVector<TQuery>::type>::type;
        return get_all<TInterface>(context);
    }
};

} // namespace Injection

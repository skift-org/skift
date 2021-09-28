#pragma once

#include <libinjection/Context.h>

namespace Injection
{

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

} // namespace Injection

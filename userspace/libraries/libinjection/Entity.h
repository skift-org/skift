#pragma once

#include <libutils/RefCounted.h>

namespace Injection
{

#define INJECTOR(__constructor)         \
    using __INJECTOR__ = __constructor; \
    __constructor

template <typename T>
concept ConstructorInjectable = requires(typename T::__INJECTOR__ a)
{
    a;
};

struct Context;

template <typename T>
concept ContextInjectable = requires(Context &context)
{
    T{context};
};

struct Entity : public AnyRef
{
};

} // namespace Injection

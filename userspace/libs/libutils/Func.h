#pragma once

#include <assert.h>

#include <libutils/OwnPtr.h>
#include <libutils/Std.h>
#include <libutils/Traits.h>

namespace Utils
{

template <typename>
struct Func;

template <typename Out, typename... In>
struct Func<Out(In...)>
{
private:
    struct CallableWrapperBase
    {
        virtual ~CallableWrapperBase() {}
        virtual Out call(In...) const = 0;
    };

    template <typename TCallable>
    struct CallableWrapper final : public CallableWrapperBase
    {
        TCallable callable;

        NONCOPYABLE(CallableWrapper);

        explicit CallableWrapper(TCallable &&callable)
            : callable{std::move(callable)}
        {
        }

        Out call(In... in) const final override
        {
            return callable(std::forward<In>(in)...);
        }
    };

    OwnPtr<CallableWrapperBase> _wrapper;

public:
    Func() = default;
    Func(nullptr_t) {}

    template <
        typename TCallable,
        typename = typename EnableIf<!(IsPointer<TCallable>::value && IsFunction<typename RemovePointer<TCallable>::Type>::value) && IsRvalueReference<TCallable &&>::value>::Type>
    Func(TCallable &&callable)
        : _wrapper{own<CallableWrapper<TCallable>>(std::move(callable))}
    {
    }

    template <
        typename TFunction,
        typename = typename EnableIf<IsPointer<TFunction>::value && IsFunction<typename RemovePointer<TFunction>::Type>::value>::Type>
    Func(TFunction function)
        : _wrapper{own<CallableWrapper<TFunction>>(std::move(function))}
    {
    }

    Out operator()(In... in) const
    {
        assert(_wrapper);
        return _wrapper->call(std::forward<In>(in)...);
    }

    explicit operator bool() const { return _wrapper; }

    template <
        typename TCallable,
        typename = typename EnableIf<!(IsPointer<TCallable>::value && IsFunction<typename RemovePointer<TCallable>::Type>::value) && IsRvalueReference<TCallable &&>::value>::Type>
    Func &operator=(TCallable &&callable)
    {
        _wrapper = own<CallableWrapper<TCallable>>(std::move(callable));
        return *this;
    }

    template <
        typename TFunction,
        typename = typename EnableIf<IsPointer<TFunction>::value && IsFunction<typename RemovePointer<TFunction>::Type>::value>::Type>
    Func &operator=(TFunction function)
    {
        _wrapper = own<CallableWrapper<TFunction>>(std::move(function));
        return *this;
    }

    Func &operator=(nullptr_t)
    {
        _wrapper = nullptr;
        return *this;
    }
};

} // namespace Utils

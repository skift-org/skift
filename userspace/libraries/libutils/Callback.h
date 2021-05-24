#pragma once

#include <libutils/OwnPtr.h>
#include <libutils/Std.h>
#include <libutils/Traits.h>

#include <assert.h>

namespace Utils
{

template <typename>
class Callback;

template <typename Out, typename... In>
class Callback<Out(In...)>
{
private:
    class CallableWrapperBase
    {
    public:
        virtual ~CallableWrapperBase() {}
        virtual Out call(In...) const = 0;
    };

    template <typename CallableType>
    class CallableWrapper final : public CallableWrapperBase
    {
    private:
        CallableType _callable;

    public:
        explicit CallableWrapper(CallableType &&callable)
            : _callable(std::move(callable))
        {
        }

        CallableWrapper(const CallableWrapper &) = delete;
        CallableWrapper &operator=(const CallableWrapper &) = delete;

        Out call(In... in) const final override
        {
            return _callable(std::forward<In>(in)...);
        }
    };

    OwnPtr<CallableWrapperBase> _wrapper;

public:
    Callback() = default;
    Callback(nullptr_t) {}

    template <typename CallableType, class = typename EnableIf<!(IsPointer<CallableType>::value && IsFunction<typename RemovePointer<CallableType>::Type>::value) && IsRvalueReference<CallableType &&>::value>::Type>
    Callback(CallableType &&callable)
        : _wrapper(own<CallableWrapper<CallableType>>(std::move(callable)))
    {
    }

    template <typename FunctionType, class = typename EnableIf<IsPointer<FunctionType>::value && IsFunction<typename RemovePointer<FunctionType>::Type>::value>::Type>
    Callback(FunctionType f)
        : _wrapper(own<CallableWrapper<FunctionType>>(std::move(f)))
    {
    }

    Out operator()(In... in) const
    {
        assert(_wrapper);
        return _wrapper->call(std::forward<In>(in)...);
    }

    explicit operator bool() const { return _wrapper; }

    template <typename CallableType, class = typename EnableIf<!(IsPointer<CallableType>::value && IsFunction<typename RemovePointer<CallableType>::Type>::value) && IsRvalueReference<CallableType &&>::value>::Type>
    Callback &operator=(CallableType &&callable)
    {
        _wrapper = own<CallableWrapper<CallableType>>(std::move(callable));
        return *this;
    }

    template <typename FunctionType, class = typename EnableIf<IsPointer<FunctionType>::value && IsFunction<typename RemovePointer<FunctionType>::Type>::value>::Type>
    Callback &operator=(FunctionType f)
    {
        _wrapper = own<CallableWrapper<FunctionType>>(std::move(f));
        return *this;
    }

    Callback &operator=(nullptr_t)
    {
        _wrapper = nullptr;
        return *this;
    }
};

} // namespace Utils

#pragma once

#include <libtest/Asserts.h>
#include <libutils/TypeId.h>
#include <libutils/Vector.h>

namespace Injection
{

struct Container;

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

} // namespace Injection

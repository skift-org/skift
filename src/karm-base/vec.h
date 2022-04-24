#pragma once

#include <karm-base/inert.h>
#include <karm-base/opt.h>
#include <karm-base/std.h>

namespace Karm::Base
{

template <typename T>
struct Vec
{
    Inert<T> *_buf = nullptr;
    size_t _cap = 0;
    size_t _len = 0;

    constexpr Vec() = default;

    Vec(size_t cap) : _cap(cap)
    {
        _buf = new Inert<T>[cap];
    }

    Vec(Vec const &other)
    {
        _cap = other._cap;
        _len = other._len;
        _buf = new Inert<T>[_cap];
        for (size_t i = 0; i < _len; i++)
        {
            _buf[i].construct(std::move(other._buf[i]));
        }
    }

    Vec(Vec &&other)
    {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
    }

    Vec(std::initializer_list<T> &&list)
    {
        _cap = list.size();
        _len = _cap;
        _buf = new Inert<T>[_cap];
        for (size_t i = 0; i < _len; i++)
        {
            _buf[i].construct(std::forward<T>(list[i]));
        }
    }

    ~Vec()
    {
        for (size_t i = 0; i < _len; i++)
        {
            _buf[i].destroy();
        }

        delete[] _buf;
    }

    Vec &operator=(Vec const &other)
    {
        return *this = Vec(other);
    }

    Vec &operator=(Vec &&other)
    {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);

        return *this;
    }

    void ensure(size_t cap)
    {
        if (cap <= _cap)
            return;

        Vec tmp(cap);
        for (size_t i = 0; i < _len; i++)
        {
            tmp._buf[i].construct(_buf[i].move());
        }

        std::swap(_buf, tmp._buf);
        std::swap(_cap, tmp._cap);
    }

    void insert(size_t index, T const &value)
    {
        ensure(_len + 1);
        for (size_t i = _len; i > index; i--)
        {
            _buf[i] = _buf[i - 1].move();
        }
        _buf[index].construct(value);
        _len++;
    }

    void insert(size_t index, T &&value)
    {
        ensure(_len + 1);
        for (size_t i = _len; i > index; i--)
        {
            _buf[i] = _buf[i - 1].move();
        }
        _buf[index].construct(std::forward<T>(value));
        _len++;
    }

    T remove(size_t index)
    {
        T tmp = _buf[index].move();
        for (size_t i = index; i < _len - 1; i++)
        {
            _buf[i] = _buf[i + 1].move();
        }
        _len--;
        return tmp;
    }

    void clear()
    {
        for (size_t i = 0; i < _len; i++)
        {
            _buf[i].destroy();
        }
        _len = 0;
    }

    void push(T const &value)
    {
        insert(_len, value);
    }

    void push(T &&value)
    {
        insert(_len, std::forward<T>(value));
    }

    Opt<T> pop()
    {
        if (_len == 0)
            return NONE;

        return remove(_len - 1);
    }

    T *begin()
    {
        return &_buf[0].unwrap();
    }

    T *end()
    {
        return begin() + _len;
    }

    size_t len() const
    {
        return _len;
    }
};

} // namespace Karm::Base

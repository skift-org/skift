#pragma once

#include <karm-meta/pack.h>

namespace Karm::Base
{

template <typename... Ts>
struct Var
{
    alignas(max(alignof(Ts)...)) char _buf[max(sizeof(Ts)...)];

    uint8_t _type;

    Var() = delete;

    template <Meta::Contains<Ts...> T>
    Var(T const &value) : _type(Meta::index_of<T, Ts...>())
    {
        new (_buf) T(value);
    }

    template <Meta::Contains<Ts...> T>
    Var(T &&value) : _type(Meta::index_of<T, Ts...>())
    {
        new (_buf) T(std::move(value));
    }

    Var(Var const &other) : _type(other._type)
    {
        Meta::index_cast<Ts...>(
            _type, other._buf,
            [this]<typename T>(T const &ptr)
            { new (_buf) T(ptr); });
    }

    Var(Var &&other) : _type(other._type)
    {
        Meta::index_cast<Ts...>(_type, other._buf, [this]<typename T>(T &ptr)
                                { new (_buf) T(std::move(ptr)); });
    }

    ~Var()
    {
        Meta::index_cast<Ts...>(_type, _buf, []<typename T>(T &ptr)
                                { ptr.~T(); });
    }

    template <Meta::Contains<Ts...> T>
    Var &operator=(T const &value)
    {
        return *this = Var(value);
    }

    template <Meta::Contains<Ts...> T>
    Var &operator=(T &&value)
    {
        Meta::index_cast<Ts...>(_type, _buf, []<typename U>(U &ptr)
                                { ptr.~U(); });

        _type = Meta::index_of<T, Ts...>();
        new (_buf) T(std::move(value));

        return *this;
    }

    Var &operator=(Var const &other) { return *this = Var(other); }

    Var &operator=(Var &&other)
    {
        Meta::index_cast<Ts...>(_type, _buf, []<typename T>(T &ptr)
                                { ptr.~T(); });

        _type = other._type;

        Meta::index_cast<Ts...>(_type, other._buf, [this]<typename T>(T &ptr)
                                { new (_buf) T(std::move(ptr)); });

        return *this;
    }

    template <Meta::Contains<Ts...> T>
    T unwrap()
    {
        return std::move(*static_cast<T *>(_buf));
    }

    template <Meta::Contains<Ts...> T>
    void with(auto visitor)
    {
        if (_type == Meta::index_of<T, Ts...>())
        {
            visitor(*static_cast<T *>(_buf));
        }
    }

    void visit(auto visitor)
    {
        Meta::index_cast<Ts...>(_type, _buf,
                                [&]<typename U>(U *ptr)
                                { visitor(*ptr); });
    }

    template <Meta::Contains<Ts...> T>
    bool is()
    {
        return _type == Meta::index_of<T, Ts...>();
    }
};

} // namespace Karm::Base

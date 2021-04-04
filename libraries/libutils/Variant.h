#pragma once

#include <libutils/Assert.h>
#include <libutils/Move.h>
#include <libutils/Traits.h>
#include <libutils/TypeId.h>

namespace Utils
{

template <size_t arg1, size_t... others>
struct static_max;

template <size_t arg>
struct static_max<arg>
{
    static const size_t value = arg;
};

template <size_t arg1, size_t arg2, size_t... others>
struct static_max<arg1, arg2, others...>
{
    static const size_t value = arg1 >= arg2 ? static_max<arg1, others...>::value : static_max<arg2, others...>::value;
};

template <typename... Ts>
struct VariantOperations;

template <>
struct VariantOperations<>
{
    inline static void destroy(TypeId, void *) {}
    inline static void move(TypeId, void *, void *) {}
    inline static void copy(TypeId, const void *, void *) {}

    template <typename TVisitor>
    inline static void visit(TVisitor &, TypeId, void *) {}
};

template <typename First, typename... Ts>
struct VariantOperations<First, Ts...>
{
    inline static void destroy(TypeId type, void *storage)
    {
        if (type == GetTypeId<First>())
        {    
            reinterpret_cast<First *>(storage)->~First();
        }
        else
        {   
            VariantOperations<Ts...>::destroy(type, storage);
        }
    }

    inline static void move(TypeId type, void *old_v, void *new_v)
    {
        if (type == GetTypeId<First>())
        {
            new (new_v) First(move(*reinterpret_cast<First *>(old_v)));
        }
        else
        {
            VariantOperations<Ts...>::move(type, old_v, new_v);
        }
    }

    inline static void copy(TypeId type, const void *old_v, void *new_v)
    {
        if (type == GetTypeId<First>())
        {   
            new (new_v) First(*reinterpret_cast<const First *>(old_v));
        }
        else
        {   
            VariantOperations<Ts...>::copy(type, old_v, new_v);
        }
    }

    template <typename TVisitor>
    inline static void visit(TVisitor &visitor, TypeId type, void *storage)
    {
        if (type == GetTypeId<First>())
        {
            visitor(*reinterpret_cast<First *>(storage));
        }
        else
        {
            VariantOperations<Ts...>::visit(visitor, type, storage);
        }
    }
};

template <size_t Len, size_t Align>
struct AlignedStorage
{
    alignas(Align) unsigned char data[Len];
};

template <typename... Ts>
struct Variant
{
private:
    static const size_t data_size = static_max<sizeof(Ts)...>::value;
    static const size_t data_align = static_max<alignof(Ts)...>::value;

    using Operations = VariantOperations<Ts...>;

    TypeId _type = INVALID_TYPE_ID;
    AlignedStorage<data_size, data_align> _storage;

public:
    Variant()
    {
        _type = GetTypeId<typename First<Ts...>::type>();
        new (&_storage) typename First<Ts...>::type();
    }

    template <typename T>
    requires In<T, Ts...>
    Variant(T value)
    {
        _type = GetTypeId<T>();
        new (&_storage) T(value);
    }

    Variant(const Variant<Ts...> &other) : _type(other._type)
    {
        Operations::copy(other._type, &other._storage, &_storage);
    }

    Variant(Variant<Ts...> &&other) : _type(other._type)
    {
        Operations::move(other._type, &other._storage, &_storage);
    }

    ~Variant()
    {
        Operations::destroy(_type, &_storage);
    }

    Variant<Ts...> &operator=(const Variant<Ts...> &other)
    {
        Operations::destroy(_type, &_storage);
        _type = other._type;
        Operations::copy(_type, &_storage, &other._storage);

        return *this;
    }

    Variant<Ts...> &operator=(Variant<Ts...> &&other)
    {
        if (this != &other)
        {
            swap(_type, other._type);
            swap(_storage, other._storage);
        }

        return *this;
    }

    bool present()
    {
        return (_type != INVALID_TYPE_ID);
    }

    template <typename T>
    bool is()
    {
        return (_type == GetTypeId<T>());
    }

    template <typename T, typename... Args>
    void set(Args &&...args)
    {
        // First we destroy the current contents
        Operations::destroy(_type, &_storage);
        new (&_storage) T(forward<Args>(args)...);
        _type = GetTypeId<T>();
    }

    template <typename T>
    T &get()
    {
        Assert::equal(_type, GetTypeId<T>());
        return *reinterpret_cast<T *>(&_storage);
    }

    template <typename T, typename Callback>
    void with(Callback callback)
    {
        if (is<T>())
        {
            callback(get<T>());
        }
    }

    template <typename TVisitor>
    void visit(TVisitor &visitor)
    {
        Operations::visit(visitor, _type, &_storage);
    }
};

template <class... Ts>
struct Visitor : Ts...
{
    using Ts::operator()...;
};

template<class... Ts> Visitor(Ts...) -> Visitor<Ts...>;

} // namespace Utils

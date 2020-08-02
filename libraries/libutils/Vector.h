#pragma once

#include <type_traits>

#include <libsystem/Common.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>
#include <libutils/RefPtr.h>

template <typename T>
class Vector
{
private:
    T *_storage{};
    size_t _count{};
    size_t _capacity{};

public:
    T *raw_storage() { return _storage; }

    size_t count() const { return _count; }
    bool empty() const { return _count == 0; }
    bool any() const { return !empty(); }

    Vector() : Vector(16) {}

    Vector(size_t capacity)
    {
        ensure_capacity(capacity);
    }

    Vector(AdoptTag, T *storage, size_t size)
        : _storage(storage),
          _count(size),
          _capacity(size)
    {
    }

    Vector(Vector &other)
    {
        ensure_capacity(other.count());
        _count = other.count();

        if constexpr (std::is_trivially_copyable_v<T>)
        {
            memcpy(_storage, other._storage, sizeof(T) * _count);
        }
        else
        {
            for (size_t i = 0; i < _count; i++)
            {
                new (&_storage[i]) T(other._storage[i]);
            }
        }
    }

    Vector(Vector &&other)
    {
        _storage = other._storage;
        _capacity = other._capacity;
        _count = other.count();

        other._storage = nullptr;
        other._capacity = 0;
        other._count = 0;
    }

    ~Vector()
    {
        clear();

        if (_storage)
            delete[] _storage;
    }

    T &operator[](size_t index)
    {
        assert(index < _count);

        return _storage[index];
    }

    void clear()
    {
        for (size_t i = 0; i < _count; i++)
        {
            _storage[i].~T();
        }

        _count = 0;
    }

    template <typename Callback>
    void foreach (Callback callback)
    {
        for (size_t i = 0; i < _count; i++)
        {
            if (callback(_storage[i]) == Iteration::STOP)
            {
                return;
            }
        }
    }

    template <typename Comparator>
    void sort(Comparator comparator)
    {
        for (size_t i = 0; i < _count - 1; i++)
        {
            for (size_t j = i + 1; j < _count; j++)
            {
                if (comparator(_storage[i], _storage[j]) > 0)
                {
                    swap(_storage[i], _storage[j]);
                }
            }
        }
    }

    void ensure_capacity(size_t capacity)
    {
        if (!_storage)
        {
            _storage = new T[capacity];
            _capacity = capacity;
            _count = 0;

            return;
        }

        capacity = MAX(_count, capacity);

        if (capacity <= _capacity)
            return;

        size_t new_capacity = _capacity;

        while (new_capacity <= capacity)
        {
            new_capacity *= 1.25;
        }

        T *new_storage = new T[new_capacity];

        for (size_t i = 0; i < _count; i++)
        {
            new_storage[i] = move(_storage[i]);
        }

        delete[] _storage;
        _storage = new_storage;
        _capacity = new_capacity;

        _count++;
    }

    void grow()
    {
        if (_count + 1 >= _capacity)
        {
            size_t new_capacity = _capacity * 1.25;
            T *new_storage = new T[new_capacity];

            for (size_t i = 0; i < _count; i++)
            {
                new_storage[i] = move(_storage[i]);
            }

            delete[] _storage;
            _storage = new_storage;
            _capacity = new_capacity;
        }

        _count++;
    }

    void shrink()
    {
        _count--;

        if (_count < _capacity * 0.75)
        {
            size_t new_capacity = _capacity * 0.75;
            T *new_storage = new T[new_capacity];

            for (size_t i = 0; i < _count; i++)
            {
                new_storage[i] = move(_storage[i]);
            }

            delete[] _storage;
            _storage = new_storage;
            _capacity = new_capacity;
        }
    }

    void insert(size_t index, T value)
    {
        grow();

        for (size_t j = _count; j > index; j--)
        {
            _storage[j] = move(_storage[j - 1]);
        }

        _storage[index] = move(value);
    }

    void insert_sorted(T value)
    {
        size_t insert_index = 0;

        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] < value)
            {
                insert_index = i;
            }
            else
            {
                break;
            }
        }

        insert(insert_index, value);
    }

    template <typename Comparator>
    void insert_sorted(T value, Comparator comparator)
    {
        size_t insert_index = 0;

        for (size_t i = 0; i < _count; i++)
        {
            if (comparator(_storage[i], value))
            {
                insert_index = i;
            }
            else
            {
                break;
            }
        }

        insert(insert_index, value);
    }

    void remove_index(size_t index)
    {
        _storage[index].~T();

        for (size_t j = index + 1; j < _count; j++)
        {
            _storage[j - 1] = move(_storage[j]);
        }

        shrink();
    }

    void remove_value(const T &value)
    {
        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] == value)
            {
                remove_index(i);
                return;
            }
        }
    }

    void remove_all_value(const T &value)
    {
        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] == value)
            {
                remove_index(i);
            }
        }
    }

    template <typename MatchCallback>
    void remove_all_match(MatchCallback match)
    {
        for (size_t i = 0; i < _count; i++)
        {
            while (match(_storage[i]) && i < _count)
            {
                remove_index(i);
            }
        }
    }

    void push(T value)
    {
        insert(0, value);
    }

    void push_back(T value)
    {
        insert(_count, value);
    }

    T pop()
    {
        assert(_count > 0);

        T value = move(_storage[0]);

        remove_index(0);

        return value;
    }

    T pop_back()
    {
        assert(_count > 0);

        T value = move(_storage[_count - 1]);

        remove_index(_count - 1);

        return value;
    }

    T &peek() const
    {
        assert(_count > 0);
        return _storage[0];
    }

    T &peek_back() const
    {
        assert(_count > 0);
        return _storage[_count - 1];
    }

    bool containe(const T &value) const
    {
        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] == value)
            {
                return true;
            }
        }

        return false;
    }
};

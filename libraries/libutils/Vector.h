#pragma once

#include <type_traits>

#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>

#include <libutils/Iteration.h>
#include <libutils/New.h>
#include <libutils/RefPtr.h>

template <typename T>
class Vector
{
private:
    T *_storage = nullptr;
    size_t _count = 0;
    size_t _capacity = 0;

    void remove_index_but_dont_destroy(size_t index)
    {
        for (size_t j = index + 1; j < _count; j++)
        {
            new (&_storage[j - 1]) T(move(_storage[j]));
        }

        shrink();
    }

public:
    size_t count() const { return _count; }

    bool empty() const { return _count == 0; }

    bool any() const { return !empty(); }

    T *raw_storage() { return _storage; }

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
            free(_storage);
    }

    T &operator[](size_t index)
    {
        assert(index < _count);

        return _storage[index];
    }

    void clear()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = 0; i < _count; i++)
            {
                _storage[i].~T();
            }
        }

        _count = 0;
    }

    template <typename Callback>
    Iteration foreach (Callback callback)
    {
        for (size_t i = 0; i < _count; i++)
        {
            if (callback(_storage[i]) == Iteration::STOP)
            {
                return Iteration::STOP;
            }
        }

        return Iteration::CONTINUE;
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
        capacity = MAX(capacity, 16);

        if (!_storage)
        {
            _storage = reinterpret_cast<T *>(calloc(capacity, sizeof(T)));
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
            new_capacity += new_capacity / 4;
        }

        T *new_storage = reinterpret_cast<T *>(calloc(new_capacity, sizeof(T)));

        for (size_t i = 0; i < _count; i++)
        {
            new (&new_storage[i]) T(move(_storage[i]));
        }

        free(_storage);
        _storage = new_storage;
        _capacity = new_capacity;

        _count++;
    }

    void grow()
    {
        if (_count + 1 >= _capacity)
        {
            size_t new_capacity = _capacity + _capacity / 4;
            T *new_storage = reinterpret_cast<T *>(calloc(new_capacity, sizeof(T)));

            for (size_t i = 0; i < _count; i++)
            {
                new (&new_storage[i]) T(move(_storage[i]));
            }

            free(_storage);
            _storage = new_storage;
            _capacity = new_capacity;
        }

        _count++;
    }

    void shrink()
    {
        _count--;

        size_t new_capacity = _capacity - _capacity / 4;

        if (MAX(_count, 16) < new_capacity)
        {
            T *new_storage = reinterpret_cast<T *>(calloc(new_capacity, sizeof(T)));

            for (size_t i = 0; i < _count; i++)
            {
                new (&new_storage[i]) T(move(_storage[i]));
            }

            free(_storage);
            _storage = new_storage;
            _capacity = new_capacity;
        }
    }

    T &insert(size_t index, T value)
    {
        assert(index <= _count);

        grow();

        for (size_t j = _count; j > index; j--)
        {
            new (&_storage[j]) T(move(_storage[j - 1]));
        }

        new (&_storage[index]) T(move(value));

        return _storage[index];
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
        assert(index < _count);

        _storage[index].~T();
        remove_index_but_dont_destroy(index);
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

    T take_at(size_t index)
    {
        assert(index < _count);

        T copy = move(_storage[index]);
        remove_index(index);
        return copy;
    }

    T &push(T value)
    {
        return insert(0, value);
    }

    T &push_back(T value)
    {
        return insert(_count, value);
    }

    void push_back_many(Vector<T> &values)
    {
        for (size_t i = 0; i < values.count(); i++)
        {
            push_back(values[i]);
        }
    }

    T pop()
    {
        assert(_count > 0);

        T value = move(_storage[0]);

        remove_index_but_dont_destroy(0);

        return value;
    }

    T pop_back()
    {
        assert(_count > 0);

        T value = move(_storage[_count - 1]);
        remove_index_but_dont_destroy(_count - 1);

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

    bool contains(const T &value) const
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

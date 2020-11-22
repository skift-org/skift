#pragma once

#include <type_traits>

#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>

#include <libutils/Iteration.h>
#include <libutils/New.h>
#include <libutils/RefPtr.h>

template <typename T>
void typed_copy(T *destination, T *source, size_t count)
{
    if constexpr (std::is_trivially_copyable_v<T>)
    {
        memcpy(destination, source, sizeof(T) * count);
    }
    else
    {
        for (size_t i = 0; i < count; i++)
        {
            new (&destination[i]) T(source[i]);
        }
    }
}

template <typename T>
void typed_move(T *destination, T *source, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        if (destination <= source)
        {
            new (&destination[i]) T(move(source[i]));
        }
        else
        {
            new (&destination[count - i - 1]) T(move(source[count - i - 1]));
        }
    }
}

template <typename T>
class Vector
{
private:
    T *_storage = nullptr;
    size_t _count = 0;
    size_t _capacity = 0;

public:
    size_t count() const { return _count; }

    bool empty() const { return _count == 0; }

    bool any() const { return !empty(); }

    T *raw_storage() { return _storage; }

    Vector() :
        Vector(16) {}

    Vector(size_t capacity)
    {
        ensure_capacity(capacity);
    }

    Vector(AdoptTag, T *storage, size_t size) :
        _storage(storage),
        _count(size),
        _capacity(size)
    {
    }

    Vector(const Vector &other)
    {
        ensure_capacity(other.count());

        _count = other.count();
        typed_copy(_storage, other._storage, _count);
    }

    Vector(Vector &&other)
    {
        swap(_storage, other._storage);
        swap(_count, other._count);
        swap(_capacity, other._capacity);
    }

    ~Vector()
    {
        clear();

        if (_storage)
            free(_storage);
    }

    Vector &operator=(const Vector &other)
    {
        if (this != &other)
        {
            clear();

            ensure_capacity(other.count());
            _count = other.count();
            typed_copy(_storage, other._storage, _count);
        }

        return *this;
    }

    Vector &operator=(Vector &&other)
    {
        if (this != &other)
        {
            swap(_storage, other._storage);
            swap(_count, other._count);
            swap(_capacity, other._capacity);
        }

        return *this;
    }

    T &operator[](size_t index)
    {
        assert(index < _count);

        return _storage[index];
    }

    const T &operator[](size_t index) const
    {
        assert(index < _count);

        return _storage[index];
    }

    bool operator==(const Vector &other) const
    {
        if (this == &other)
        {
            return true;
        }

        if (_count != other._count)
        {
            return false;
        }

        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] != other._storage[i])
            {
                return false;
            }
        }

        return true;
    }

    void clear()
    {
        if (!_storage)
        {
            return;
        }

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
    Iteration foreach (Callback callback) const
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
        {
            return;
        }

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
                _storage[i].~T();
            }

            free(_storage);
            _storage = new_storage;
            _capacity = new_capacity;
        }
    }

    T &insert(size_t index, const T &value)
    {
        return insert(index, T(value));
    }

    T &insert(size_t index, T &&value)
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

        for (size_t i = index; i < _count - 1; ++i)
        {
            new (&_storage[i]) T(move(_storage[i + 1]));
            _storage[i + 1].~T();
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
    bool remove_all_match(MatchCallback match)
    {
        bool has_removed_something = false;

        for (size_t i = 0; i < _count; i++)
        {
            while (i < _count && match(_storage[i]))
            {
                remove_index(i);
                has_removed_something = true;
            }
        }

        return has_removed_something;
    }

    T take_at(size_t index)
    {
        assert(index < _count);

        T copy = move(_storage[index]);
        remove_index(index);
        return copy;
    }

    T &pash(const T &value)
    {
        return push(T(value));
    }

    T &push(T &&value)
    {
        return insert(0, move(value));
    }

    T &push_back(const T &value)
    {
        return push_back(T(value));
    }

    T &push_back(T &&value)
    {
        return insert(_count, move(value));
    }

    void push_back_many(const Vector<T> &values)
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

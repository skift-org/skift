#pragma once

#include <libsystem/Common.h>

#include <libutils/Move.h>
#include <libutils/RefPtr.h>
#include <libutils/SliceStorage.h>

class Slice
{
private:
    SliceStorage *_storage = nullptr;
    const void *_start = nullptr;
    size_t _size = 0;

public:
    bool any() const { return _size > 0; }

    size_t size() const { return _size; }

    const void *start() const
    {
        return _start;
    }

    const void *end() const
    {
        return reinterpret_cast<const char *>(start()) + _size;
    }

    Slice()
    {
    }

    Slice(SliceStorage &storage, size_t offset, size_t size) : _storage(&storage)
    {
        ref_if_not_null(_storage);

        _start = _storage->start();
        _start = static_cast<const void *>(static_cast<const char *>(_start) + offset);
        _size = size;
    }

    Slice(RefPtr<SliceStorage> &&storage) : _storage(storage.give_ref())
    {
        _start = _storage->start();
        _size = _storage->size();
    }

    Slice(const void *start, size_t size)
    {
        _start = start;
        _size = size;
    }

    Slice(const Slice &other) : _storage(other._storage)
    {
        ref_if_not_null(_storage);

        _start = other.start();
        _size = other.size();
    }

    Slice(Slice &&other) : _storage(other.give_storage())
    {
        _start = other.start();
        _size = other.size();
    }

    ~Slice()
    {
        deref_if_not_null(_storage);
    }

    Slice &operator=(const Slice &other)
    {
        if (_storage != other.naked_storage())
        {
            deref_if_not_null(_storage);
            _storage = other.naked_storage();
            ref_if_not_null(_storage);
        }

        return *this;
    }

    Slice &operator=(Slice &&other)
    {
        if (this != &other)
        {
            deref_if_not_null(_storage);
            _storage = other.give_storage();
        }

        return *this;
    }

    Slice slice(size_t offset, size_t size) const
    {
        if (_storage != nullptr)
        {
            return {*_storage, offset, size};
        }
        else
        {
            auto start = static_cast<const void *>(static_cast<const char *>(_start) + offset);
            return {start, size};
        }
    }

    [[nodiscard]] SliceStorage *give_storage()
    {
        SliceStorage *storage = _storage;
        _storage = nullptr;
        return storage;
    }

    SliceStorage *naked_storage() const
    {
        return _storage;
    }
};

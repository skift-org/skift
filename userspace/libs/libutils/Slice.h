#pragma once

#include <libutils/Prelude.h>

#include <libutils/RefPtr.h>
#include <libutils/SliceStorage.h>
#include <libutils/Std.h>

struct Slice :
    public RawStorage
{
private:
    RefPtr<Storage> _storage;

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

    Slice(RefPtr<Storage> storage)
    {
        _storage = storage;
        _start = _storage->start();
        _size = _storage->size();
    }

    Slice(RefPtr<Storage> storage, size_t offset, size_t size)
    {
        _storage = storage;
        _start = _storage->start();
        _start = static_cast<const void *>(static_cast<const char *>(_start) + offset);
        _size = size;
    }

    Slice(const char *start)
    {
        _storage = nullptr;
        _start = start;
        _size = strlen(start);
    }

    Slice(const void *start, size_t size)
    {
        _storage = nullptr;
        _start = start;
        _size = size;
    }

    Slice(RawStorage &obj)
    {
        _storage = obj.storage();
        _start = _storage->start();
        _size = _storage->size();
    }

    Slice slice(size_t offset, size_t size) const
    {
        if (_storage != nullptr)
        {
            return {_storage, offset, size};
        }
        else
        {
            auto start = static_cast<const void *>(static_cast<const char *>(_start) + offset);
            return {start, size};
        }
    }

    RefPtr<Storage> storage() override
    {
        return _storage;
    }
};
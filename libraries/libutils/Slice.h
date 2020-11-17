#pragma once

#include <libsystem/Common.h>

#include <libutils/Move.h>
#include <libutils/RefPtr.h>
#include <libutils/SliceStorage.h>

class Slice
{
private:
    RefPtr<SliceStorage> _storage = nullptr;
    size_t _offset = 0;
    size_t _size = 0;

public:
    bool any() { return _size > 0; }

    size_t size() { return _size; }

    const void *start()
    {
        return reinterpret_cast<const char *>(_storage->start()) + _offset;
    }

    const void *end()
    {
        return reinterpret_cast<const char *>(start()) + _size;
    }

    Slice()
    {
    }

    Slice(RefPtr<SliceStorage> storage)
        : _storage(storage),
          _offset(0),
          _size(storage->size())
    {
    }

    Slice(RefPtr<SliceStorage> storage, size_t offset, size_t size)
        : _storage(storage),
          _offset(offset),
          _size(size)
    {
        assert(offset <= storage->size());
        assert(offset + size <= storage->size());
    }

    Slice slice(size_t offset, size_t size)
    {
        // FIXME: bound checking.
        return {_storage, _offset + offset, size};
    }
};

#pragma once

#include <libutils/RefCounted.h>
#include <string.h>

class SliceStorage : public RefCounted<SliceStorage>
{
private:
    void *_data = nullptr;
    size_t _size = 0;
    bool _owned = false;

public:
    void *start()
    {
        return _data;
    }

    void *end()
    {
        return reinterpret_cast<char *>(_data) + _size;
    }

    const void *start() const
    {
        return _data;
    }

    const void *end() const
    {
        return reinterpret_cast<const char *>(_data) + _size;
    }

    size_t size()
    {
        return _size;
    }

    enum Mode
    {
        ADOPT,
        WRAP,
        COPY,
    };

    SliceStorage(size_t size)
    {
        _owned = true;
        _data = malloc(size);
        _size = size;
    }

    SliceStorage(Mode mode, void *data, size_t size)
    {
        if (mode == ADOPT)
        {
            _data = data;
            _size = size;
            _owned = true;
        }
        else if (mode == WRAP)
        {
            _data = data;
            _size = size;
            _owned = false;
        }
        else if (mode == COPY)
        {
            _data = malloc(size);
            memcpy(_data, data, size);
            _size = size;
            _owned = false;
        }
    }

    ~SliceStorage()
    {
        if (!_data)
        {
            return;
        }

        if (_owned)
        {
            free(_data);
        }

        _data = nullptr;
    }
};

#pragma once

#include <string.h>

#include <libutils/Storage.h>
#include <libutils/Tags.h>

class SliceStorage final :
    public Storage
{
private:
    void *_data = nullptr;
    size_t _size = 0;
    bool _owned = false;

public:
    using Storage::end;
    using Storage::start;

    void *start() override { return _data; }

    void *end() override { return reinterpret_cast<char *>(start()) + _size; }

    SliceStorage(size_t size)
    {
        _owned = true;
        _data = malloc(size);
        _size = size;
    }

    SliceStorage(AdoptTag, void *data, size_t size)
    {
        _data = data;
        _size = size;
        _owned = true;
    }

    SliceStorage(WrapTag, void *data, size_t size)
    {
        _data = data;
        _size = size;
        _owned = false;
    }

    SliceStorage(CopyTag, void *data, size_t size)
    {
        _data = malloc(size);
        memcpy(_data, data, size);
        _size = size;
        _owned = false;
    }

    ~SliceStorage() override
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
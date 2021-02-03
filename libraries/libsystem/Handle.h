#pragma once

#include <abi/Syscalls.h>

#include <libutils/Optional.h>
#include <libutils/ResultOr.h>
#include <libutils/String.h>

namespace system
{

class Handle
{
private:
    int _handle = HANDLE_INVALID_ID;
    Result _result = ERR_BAD_HANDLE;

    __noncopyable(Handle);

public:
    Handle(int handle) : _handle(handle), _result(handle != HANDLE_INVALID_ID ? SUCCESS : ERR_BAD_HANDLE)
    {
    }

    Handle(const String path, OpenFlag flags)
    {
        _result = hj_handle_open(&_handle, path.cstring(), path.length(), flags);
    }

    Handle(Handle &&other)
    {
        _handle = exchange_and_return_initial_value(other._handle, HANDLE_INVALID_ID);
        _result = exchange_and_return_initial_value(other._result, ERR_BAD_HANDLE);
    }

    Handle &operator=(Handle &&other)
    {
        swap(_handle, other._handle);
        swap(_result, other._result);

        return *this;
    }

    ~Handle()
    {
        if (_handle != HANDLE_INVALID_ID)
        {
            hj_handle_close(_handle);
            _handle = -1;
        }
    }

    ResultOr<size_t> read(void *buffer, size_t size)
    {
        size_t data_read = 0;
        _result = hj_handle_read(_handle, buffer, size, &data_read);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return data_read;
        }
    }

    ResultOr<size_t> write(const void *buffer, size_t size)
    {
        size_t data_written = 0;
        _result = hj_handle_write(_handle, buffer, size, &data_written);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return data_written;
        }
    }

    Result call(IOCall request, void *args)
    {
        _result = hj_handle_call(_handle, request, args);
        return _result;
    }

    Result seek(int offset, Whence whence)
    {
        _result = hj_handle_seek(_handle, offset, whence);
        return _result;
    }

    ResultOr<int> tell(Whence whence)
    {
        int offset = 0;
        _result = hj_handle_tell(_handle, whence, &offset);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return offset;
        }
    }

    ResultOr<FileState> stat()
    {
        FileState stat{};
        _result = hj_handle_stat(_handle, &stat);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return stat;
        }
    }

    ResultOr<Handle> accept()
    {
        int connection_handle;
        _result = hj_handle_accept(_handle, &connection_handle);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return Handle{connection_handle};
        }
    }
};

} // namespace system

#pragma once

#include <abi/Syscalls.h>
#include <libio/Seek.h>
#include <libsystem/process/Process.h>
#include <libutils/String.h>

namespace IO
{

class Handle :
    public RefCounted<Handle>
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
        auto resolved_path = process_resolve(path);
        _result = hj_handle_open(&_handle, resolved_path.cstring(), resolved_path.length(), flags);
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
        _result = TRY(hj_handle_read(_handle, buffer, size, &data_read));
        return data_read;
    }

    ResultOr<size_t> write(const void *buffer, size_t size)
    {
        size_t data_written = 0;
        _result = TRY(hj_handle_write(_handle, buffer, size, &data_written));
        return data_written;
    }

    Result call(IOCall request, void *args)
    {
        _result = hj_handle_call(_handle, request, args);
        return _result;
    }

    ResultOr<ssize64_t> seek(IO::SeekFrom from)
    {
        ssize64_t result_offset = 0;
        _result = TRY(hj_handle_seek(_handle, &from.position, (HjWhence)from.whence, &result_offset));
        return result_offset;
    }

    ResultOr<ssize64_t> tell()
    {
        ssize64_t position = 0;
        ssize64_t result_offset = 0;
        _result = TRY(hj_handle_seek(_handle, &position, HJ_WHENCE_CURRENT, &result_offset));
        return result_offset;
    }

    ResultOr<FileState> stat()
    {
        FileState stat{};
        _result = TRY(hj_handle_stat(_handle, &stat));
        return stat;
    }

    ResultOr<Handle> accept()
    {
        int connection_handle;
        _result = TRY(hj_handle_accept(_handle, &connection_handle));
        return Handle{connection_handle};
    }

    bool valid()
    {
        return _handle != HANDLE_INVALID_ID;
    }
};

struct RawHandle
{
    virtual RefPtr<Handle> handle() = 0;
};

} // namespace IO

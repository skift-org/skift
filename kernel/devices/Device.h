#pragma once

#include <abi/Paths.h>

#include <libutils/RefPtr.h>
#include <libutils/String.h>
#include <libutils/StringBuilder.h>
#include <libutils/Vector.h>

#include "kernel/devices/DeviceAddress.h"
#include "kernel/devices/DeviceClass.h"
#include "kernel/node/Handle.h"

class Device : public RefCounted<Device>
{
private:
    DeviceAddress _address;
    DeviceClass _klass;
    String _name;

    Vector<RefPtr<Device>> _childs{};

public:
    DeviceClass klass()
    {
        return _klass;
    }

    String name()
    {
        return _name;
    }

    String path()
    {
        return StringBuilder()
            .append(DEVICE_PATH)
            .append("/")
            .append(name())
            .finalize();
    }

    DeviceAddress address()
    {
        return _address;
    }

    void add(RefPtr<Device> child)
    {
        _childs.push_back(child);
    }

    Iteration iterate(IterationCallback<RefPtr<Device>> &callback)
    {
        return _childs.foreach ([&](auto device) {
            if (callback(device) == Iteration::STOP)
            {
                return Iteration::STOP;
            }

            return device->iterate(callback);
        });
    }

    Device(DeviceAddress address, DeviceClass klass);

    virtual ~Device(){};

    virtual int interrupt() { return -1; }

    virtual void acknowledge_interrupt() {}

    virtual void handle_interrupt() {}

    virtual bool did_fail() {
        return false;
    }

    virtual bool can_read(FsHandle &handle)
    {
        __unused(handle);

        return true;
    }

    virtual bool can_write(FsHandle &handle)
    {
        __unused(handle);

        return true;
    }

    virtual size_t size(FsHandle &handle)
    {
        __unused(handle);

        return 0;
    }

    virtual ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size)
    {
        __unused(handle);
        __unused(buffer);
        __unused(size);

        return ERR_NOT_READABLE;
    }

    virtual ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size)
    {
        __unused(handle);
        __unused(buffer);
        __unused(size);

        return ERR_NOT_WRITABLE;
    }

    virtual Result call(FsHandle &handle, IOCall request, void *args)
    {
        __unused(handle);
        __unused(request);
        __unused(args);

        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
};

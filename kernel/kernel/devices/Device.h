#pragma once

#include <abi/IOCall.h>
#include <abi/Paths.h>

#include <libutils/RefPtr.h>
#include <libutils/ResultOr.h>
#include <libutils/String.h>
#include <libutils/StringBuilder.h>
#include <libutils/Vector.h>

#include "kernel/devices/DeviceAddress.h"
#include "kernel/devices/DeviceClass.h"

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

    virtual String name()
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

    virtual bool did_fail()
    {
        return false;
    }

    virtual bool can_read()
    {
        return true;
    }

    virtual bool can_write()
    {
        return true;
    }

    virtual size_t size()
    {
        return 0;
    }

    virtual ResultOr<size_t> read(size64_t offset, void *buffer, size_t size)
    {
        UNUSED(offset);
        UNUSED(buffer);
        UNUSED(size);

        return ERR_NOT_READABLE;
    }

    virtual ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size)
    {
        UNUSED(offset);
        UNUSED(buffer);
        UNUSED(size);

        return ERR_NOT_WRITABLE;
    }

    virtual Result call(IOCall request, void *args)
    {
        UNUSED(request);
        UNUSED(args);

        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
};

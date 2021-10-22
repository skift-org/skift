#pragma once

#include <abi/IOCall.h>
#include <abi/Paths.h>

#include <libio/Format.h>
#include <libutils/Ref.h>
#include <libutils/ResultOr.h>
#include <libutils/String.h>
#include <libutils/Vec.h>

#include "system/devices/DeviceAddress.h"
#include "system/devices/DeviceClass.h"

struct Device : public Shared<Device>
{
private:
    DeviceAddress _address;
    DeviceClass _klass;
    String _name;

    Vec<Ref<Device>> _children{};

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
        return IO::format("{}/{}", DEVICE_PATH, name());
    }

    DeviceAddress address()
    {
        return _address;
    }

    void add(Ref<Device> child)
    {
        _children.push_back(child);
    }

    Iter iterate(IterFunc<Ref<Device>> &callback)
    {
        return _children.foreach([&](auto device)
            {
                if (callback(device) == Iter::STOP)
                {
                    return Iter::STOP;
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

    virtual HjResult call(IOCall request, void *args)
    {
        UNUSED(request);
        UNUSED(args);

        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
};

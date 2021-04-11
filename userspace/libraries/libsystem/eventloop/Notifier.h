#pragma once

#include <libio/Handle.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libutils/Callback.h>

class Notifier : public IO::RawHandle
{
private:
    RefPtr<IO::Handle> _handle;
    PollEvent _events;
    Callback<void()> _callback;

public:
    RefPtr<IO::Handle> handle() { return _handle; }
    PollEvent events() { return _events; }

    Notifier(RawHandle &raw_handle, PollEvent events, Callback<void()> callback)
        : _handle{raw_handle.handle()},
          _events{events},
          _callback{callback}
    {
        EventLoop::register_notifier(this);
    }

    ~Notifier()
    {
        EventLoop::unregister_notifier(this);
    }

    void invoke()
    {
        _callback();
    }
};

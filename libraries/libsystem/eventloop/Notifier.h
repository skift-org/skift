#pragma once

#include <libutils/Callback.h>

#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/io/Handle.h>

class Notifier
{
private:
    Handle *_handle;
    PollEvent _events;
    Callback<void()> _callback;

public:
    Handle *handle() { return _handle; }
    PollEvent events() { return _events; }

    Notifier(Handle *handle, PollEvent events, Callback<void()> callback)
        : _handle(handle),
          _events(events),
          _callback(callback)
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

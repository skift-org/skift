#pragma once

#include <libsystem/eventloop/EventLoop.h>
#include <libutils/Callback.h>
#include <libutils/RefCounted.h>

class Invoker
{
private:
    bool _invoke_later = false;
    Callback<void()> _callback;

public:
    Invoker(Callback<void()> callback) : _callback(callback)
    {
        EventLoop::register_invoker(this);
    }

    ~Invoker()
    {
        EventLoop::unregister_invoker(this);
    }

    bool should_be_invoke_later()
    {
        return _invoke_later;
    }

    void invoke()
    {
        _invoke_later = false;
        _callback();
    }

    void invoke_later()
    {
        _invoke_later = true;
    }

    void cancel()
    {
        _invoke_later = false;
    }
};

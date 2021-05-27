#pragma once

#include <libasync/Source.h>
#include <libio/Handle.h>
#include <libutils/Func.h>

namespace Async
{

struct Notifier :
    public IO::RawHandle,
    public Source
{
private:
    RefPtr<IO::Handle> _handle;
    PollEvent _events;
    Func<void()> _callback;

public:
    RefPtr<IO::Handle> handle() override { return _handle; }
    PollEvent events() { return _events; }

    Notifier(RawHandle &raw_handle, PollEvent events, Func<void()> callback)
        : _handle{raw_handle.handle()},
          _events{events},
          _callback{callback}
    {
        loop().register_notifier(this);
    }

    ~Notifier()
    {
        loop().unregister_notifier(this);
    }

    void invoke()
    {
        _callback();
    }
};

} // namespace Async
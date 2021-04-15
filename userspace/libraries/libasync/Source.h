#pragma once

#include <libasync/Loop.h>
#include <libutils/RefPtr.h>

namespace Async
{

class Source
{
private:
    RefPtr<Loop> _eventloop;

public:
    Loop &loop() { return *_eventloop; }

    Source()
    {
        _eventloop = Loop::the();
    }

    virtual ~Source()
    {
    }
};

} // namespace Async

#pragma once

#include <libasync/Loop.h>
#include <libutils/Ref.h>

namespace Async
{

struct Source
{
private:
    Ref<Loop> _eventloop;

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

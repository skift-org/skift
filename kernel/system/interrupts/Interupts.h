#pragma once

#include <libsystem/Common.h>

#define ASSERT_INTERRUPTS_RETAINED() assert(interrupts_retained())

#define ASSERT_INTERRUPTS_NOT_RETAINED() assert(!interrupts_retained())

void interrupts_initialize();

bool interrupts_retained();

void interrupts_enable_holding();

void interrupts_disable_holding();

void interrupts_retain();

void interrupts_release();

class InterruptsRetainer
{
private:
    NONCOPYABLE(InterruptsRetainer);
    NONMOVABLE(InterruptsRetainer);

public:
    InterruptsRetainer()
    {
        interrupts_retain();
    }

    ~InterruptsRetainer()
    {
        interrupts_release();
    }
};

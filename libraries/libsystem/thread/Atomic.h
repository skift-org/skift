#pragma once

#include <libsystem/Common.h>

void atomic_enable();

void atomic_disable();

void atomic_begin();

void atomic_end();

bool is_atomic();

#define ASSERT_ATOMIC assert(is_atomic())

class AtomicHolder
{
private:
    __noncopyable(AtomicHolder);
    __nonmovable(AtomicHolder);

public:
    AtomicHolder()
    {
        atomic_begin();
    }

    ~AtomicHolder()
    {
        atomic_end();
    }
};

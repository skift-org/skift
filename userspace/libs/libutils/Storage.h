#pragma once

#include <libutils/Ref.h>

namespace Utils
{

struct Storage : public Shared<Storage>
{
    virtual ~Storage() {}

    size_t size() { return (uintptr_t)end() - (uintptr_t)start(); }

    virtual void *end() = 0;
    virtual void *start() = 0;

    virtual const void *start() const { return const_cast<Storage *>(this)->start(); };
    virtual const void *end() const { return const_cast<Storage *>(this)->end(); }
};

struct RawStorage
{
    virtual ~RawStorage() {}

    virtual Ref<Storage> storage() = 0;
};

} // namespace Utils

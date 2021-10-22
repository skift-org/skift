#pragma once

#include <assert.h>

namespace Utils
{

template <typename T>
struct Shared
{
private:
    int _refcount = 1;

    NONCOPYABLE(Shared);
    NONMOVABLE(Shared);

public:
    Shared() {}

    virtual ~Shared()
    {
        assert(_refcount == 0);
    }

    void ref()
    {
        int refcount = __atomic_add_fetch(&_refcount, 1, __ATOMIC_SEQ_CST);
        assert(refcount >= 0);
    }

    void deref()
    {
        int refcount = __atomic_sub_fetch(&_refcount, 1, __ATOMIC_SEQ_CST);
        assert(refcount >= 0);

        if (refcount == 0)
        {
            delete static_cast<T *>(this);
        }
    }

    int refcount() const
    {
        return _refcount;
    }
};

struct AnyRef : public Shared<AnyRef>
{
    virtual ~AnyRef() = default;
};

template <typename T>
T *ref_if_not_null(T *ptr)
{
    if (ptr)
    {
        ptr->ref();
    }

    return ptr;
}

template <typename T>
T *deref_if_not_null(T *ptr)
{
    if (ptr)
    {
        ptr->deref();
    }

    return ptr;
}

} // namespace Utils

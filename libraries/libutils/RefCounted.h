#pragma once

#include <libsystem/Assert.h>
#include <libsystem/Logger.h>

template <typename T>
class RefCounted
{
private:
    int _refcount = 1;
    bool _orphan = false;

    __noncopyable(RefCounted);
    __nonmovable(RefCounted);

public:
    RefCounted() {}

    virtual ~RefCounted()
    {
        if (!_orphan)
        {
            assert(_refcount == 0);
        }
    }

    // FIXME: Find a better name for this...
    void make_orphan()
    {
        _orphan = true;
    }

    void ref()
    {
        if (!_orphan)
        {
            int refcount = __atomic_add_fetch(&_refcount, 1, __ATOMIC_SEQ_CST);
            assert(refcount >= 0);
        }
    }

    void deref()
    {
        if (!_orphan)
        {
            int refcount = __atomic_sub_fetch(&_refcount, 1, __ATOMIC_SEQ_CST);
            assert(refcount >= 0);

            if (refcount == 1)
            {
                if constexpr (requires(const T &t) {
                                  t.one_ref_left();
                              })
                {
                    this->one_ref_left();
                }
            }

            if (refcount == 0)
            {
                delete static_cast<T *>(this);
            }
        }
    }

    int refcount()
    {
        if (!_orphan)
        {
            return _refcount;
        }
        else
        {
            return 0;
        }
    }
};

template <typename T>
void ref_if_not_null(T *ptr)
{
    if (ptr)
    {
        ptr->ref();
    }
}

template <typename T>
void deref_if_not_null(T *ptr)
{
    if (ptr)
    {
        ptr->deref();
    }
}

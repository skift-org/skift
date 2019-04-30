#include <skift/__plugs__.h>
#include <skift/atomic.h>

int __plug_logger_lock()
{
    sk_atomic_begin();
    return 0;
}

int __plug_logger_unlock()
{
    sk_atomic_end();
    return 0;
}

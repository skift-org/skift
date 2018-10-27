#include <skift/atomic.h>

#include "kernel/objects.h"

kobject_t* root_object;

/* --- Handle --------------------------------------------------------------- */

kobject_handle_t handle_count = 0;

kobject_handle_t kobject_handle()
{
    kobject_handle_t handle = 0;

    ATOMIC({
        handle = handle_count++;
    });

    return handle;
}

/* --- Kernel objects ------------------------------------------------------- */

KOBJECT_CONSTRUCTOR(root)
{
    // nothing to do!
    UNUSED(object);
    return object;
}

/* --- Kernel objects constructor/destructor -------------------------------- */
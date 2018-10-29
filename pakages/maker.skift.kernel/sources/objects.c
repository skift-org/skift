#include <stdlib.h>
#include <skift/atomic.h>

#include "kernel/objects.h"

kobject_t *root_object;

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

void kobject_mount(kobject_t *parent, kobject_type_t type, kobject_t *object)
{
    UNUSED(parent);
    UNUSED(type);
    UNUSED(object);

    object->__.type = type;
    object->__.handle = kobject_handle();
}

void kobject_unmount(kobject_t *object)
{
    UNUSED(object);
}

/* --- Kernel objects constructor/destructor -------------------------------- */

KOBJECT_CONSTRUCTOR(root)
{
    // nothing to do!
    UNUSED(object);
    return object;
}

KOBJECT_DESTRUCTOR(root)
{
    UNUSED(object);
}

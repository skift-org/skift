#pragma once

#include <skift/types.h>
#include <skift/utils.h>
#include <skift/list.h>

typedef uint kobject_handle_t;
typedef uint kobject_refcount_t;

typedef enum
{
    KOBJ_KOBJECT,
    KOBJ_ROOT,

    KOBJ_PROCESS,
    KOBJ_THREAD,

    KOBJ_SHMEM,
    KOBJ_CHANNEL,
    KOBJ_MESSAGE,
} kobject_type_t;

#define KOBJECT_STRUCT(__type__, __fields__) \
    typedef PACKED(struct)                   \
    {                                        \
        struct                               \
        {                                    \
            kobject_type_t type;             \
            kobject_handle_t handle;         \
            kobject_refcount_t refcount;     \
            list_t *childs;                  \
        } __;                                \
        struct __fields__;                   \
    }                                        \
    __type__##_t;

#define KOBJECT(__type__, __fields__)    \
    KOBJECT_STRUCT(__type__, __fields__) \
    __type__##_t *__type__##_init(__type__##_t *object);

#define KOBJECT_CONSTRUCTOR(__type__)                \
    __type__##_t *__type__##()                       \
    {                                                \
        __type__##_t *object = MALLOC(__type__##_t); \
        __type__##_t *__type__##_init(object);       \
        return object;                               \
    }                                                \
    __type__##_t *__type__##_init(__type__##_t *object)

/* --- Kernel object declaration -------------------------------------------- */

KOBJECT(kobject,
        { int data; });

KOBJECT(root,
        {});

KOBJECT(process,
        {
            char name[256];
        });

KOBJECT(thread,
        {
            char name[256];
        });

KOBJECT(shmem,
        {
            char name[256];
        });

KOBJECT(channel,
        {
            char name[256];
        });

KOBJECT(message,
        {
            char name[256];
        });
#pragma once

#include <skift/types.h>
#include <skift/utils.h>
#include <skift/list.h>

typedef uint kobject_handle_t;
typedef uint kobject_refcount_t;

typedef enum
{
    kobj_kobject,
    kobj_root,

    kobj_process,
    kobj_thread,

    kobj_shmem,
    kobj_channel,
    kobj_message,
} kobject_type_t;

#define KOBJECT_STRUCT(__type__, __fields__) \
    typedef struct                           \
    {                                        \
        struct                               \
        {                                    \
            kobject_type_t type;             \
            kobject_handle_t handle;         \
            kobject_refcount_t refcount;     \
            list_t *childs;                  \
        } __;                                \
        struct __fields__;                   \
    } __type__##_t;

#define KOBJECT(__type__, __fields__)                    \
    KOBJECT_STRUCT(__type__, __fields__)                 \
    __type__##_t *__type__(kobject_t *parent);           \
    __type__##_t *__type__##_ctor(__type__##_t *object); \
    void __type__##_dtor(__type__##_t *object);

#define KOBJECT_CONSTRUCTOR(__type__)                                \
    __type__##_t *__type__(kobject_t *parent)                        \
    {                                                                \
        __type__##_t *object = MALLOC(__type__##_t);                 \
        __type__##_ctor(object);                                     \
        kobject_mount(parent, kobj_##__type__, (kobject_t *)object); \
        return object;                                               \
    }                                                                \
    __type__##_t *__type__##_ctor(__type__##_t *object)

#define KOBJECT_DESTRUCTOR(__type__)             \
    void delete_##__type__(__type__##_t *object) \
    {                                            \
        __type__##_dtor(object);                 \
        free(object);                            \
    }                                            \
    void __type__##_dtor(__type__##_t *object)

/* --- Kernel object declaration -------------------------------------------- */

KOBJECT(kobject,
        { int data; });

void kobject_mount(kobject_t *parent, kobject_type_t type, kobject_t *object);
void kobject_unmount(kobject_t *object);

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
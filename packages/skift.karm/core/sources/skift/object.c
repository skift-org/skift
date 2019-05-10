#include <skift/assert.h>
#include <skift/lock.h>

#include <skift/object.h>

#define OBJECT_MAGIC 0xBADF00CA
typedef struct 
{
    uint magic;
    lock_t lock;
    int refcount;
    object_dtor_t dtor;
} object_header_t;

#define GET_OBJECT_HEADER(__object) (((object_header_t*)(__object)) - 1) 

object_t *object(uint size, object_dtor_t dtor)
{
    object_header_t* header = malloc(sizeof(object_header_t) + size);

    header->magic = OBJECT_MAGIC;
    header->refcount = 1;
    header->dtor = dtor;
    sk_lock_init(header->lock);

    return (object_t*)(header + 1);
}

void object_lock(object_t *this)
{
    assert(this != NULL);

    object_header_t* header = GET_OBJECT_HEADER(this);

    assert(header->magic == OBJECT_MAGIC);

    sk_lock_acquire(header->lock);
}

bool object_trylock(object_t *this)
{
    assert(this != NULL);

    object_header_t* header = GET_OBJECT_HEADER(this);

    assert(header->magic == OBJECT_MAGIC);

    return sk_lock_try_acquire(header->lock);
}

void object_unlock(object_t *this)
{
    assert(this != NULL);

    object_header_t* header = GET_OBJECT_HEADER(this);

    assert(header->magic == OBJECT_MAGIC);

    sk_lock_release(header->lock);
}

object_t *object_retain(object_t *this)
{
    assert(this != NULL);

    object_header_t* header = GET_OBJECT_HEADER(this);

    assert(header->magic == OBJECT_MAGIC);

    object_lock(this);
    
    header->refcount++;

    object_unlock(this);

    return this;
}

void object_release(object_t *this)
{
    assert(this != NULL);

    object_header_t* header = GET_OBJECT_HEADER(this);

    assert(header->magic == OBJECT_MAGIC);

    object_lock(this);
    
    header->refcount++;

    if (header->refcount == 0)
    {
        if (header->dtor != NULL)
        {
            header->dtor(this);
        }

        header->magic = 0;
        free(header);
    }
    else
    {
        object_unlock(this);
    }
}

int object_refcount(object_t *this)
{
    assert(this != NULL);

    object_header_t* header = GET_OBJECT_HEADER(this);

    assert(header->magic == OBJECT_MAGIC);

    object_lock(this);
    
    int refcount = header->refcount;

    object_unlock(this);

    return refcount;
}
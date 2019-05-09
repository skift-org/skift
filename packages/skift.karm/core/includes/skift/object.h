#pragma once

#include <skift/runtime.h>

typedef void (*object_dtor_t)(void *object);

typedef void object_t;

object_t *object(uint size, object_dtor_t dtor);

void object_lock(object_t *this);
bool object_trylock(object_t *this);
void object_unlock(object_t *this);

object_t *object_retain(object_t *this);
void object_release(object_t *this);
int object_refcount(object_t *this);
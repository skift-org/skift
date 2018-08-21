#pragma once
#include "types.h"

typedef struct 
{
    string name;
    bool locked;
} lock_t;

void lock_init(lock_t* lock, string name);
void lock_acquire(lock_t* lock);
void lock_release(lock_t* lock);
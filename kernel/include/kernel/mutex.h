#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>
#include <skift/list.h>
#include "kernel/tasking.h"

typedef struct 
{
    char name[128];

    bool aquired;

    task_t* holder;
    list_t* wait_queue;
} mutex_t;

mutex_t* mutex(const char* name);
void mutex_delete(mutex_t* mutex);

bool mutex_acquire(mutex_t* m);
bool mutex_release(mutex_t* m);
#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/list.h>
#include <libsystem/runtime.h>

typedef struct
{
    char *key;
    object_t *value;
} map_keyvalue_pair_t;

typedef struct
{
    List *keyvalue_pairs;
} map_t;

typedef IterationDecision map_foreach_callback_t(map_keyvalue_pair_t *kv, void *arg, void **ret);

map_t *map(void);

bool map_get(map_t *this, const char *key, void **value);

void map_set(map_t *this, const char *key, object_t *value);

void map_remove(map_t *this, const char *key);

bool map_exist(map_t *this, const char *key);

void *map_foreach(map_t *this, map_foreach_callback_t *callback, void *arg);

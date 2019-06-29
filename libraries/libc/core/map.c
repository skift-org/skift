/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/cstring.h>
#include <skift/map.h>
#include <skift/assert.h>

/* --- Key-Value pair ------------------------------------------------------- */

void map_keyvalue_pair_delete(map_keyvalue_pair_t *this);

map_keyvalue_pair_t *map_keyvalue_pair(void)
{
    map_keyvalue_pair_t *this = OBJECT(map_keyvalue_pair);

    this->key = NULL;
    this->value = NULL;

    return this;
}

void map_keyvalue_pair_delete(map_keyvalue_pair_t *this)
{
    free(this->key);
    object_release(this->value);
}

const char *map_keyvalue_pair_get_key(map_keyvalue_pair_t *this)
{
    assert(this);

    return this->key;
}

void map_keyvalue_pair_set_key(map_keyvalue_pair_t *this, const char *key)
{
    assert(this);
    assert(key);

    if (this->key)
    {
        free(this->key);
    }

    this->key = strdup(key);
}

object_t *map_keyvalue_pair_get_value(map_keyvalue_pair_t *this)
{
    assert(this);

    return this->value;
}

void map_keyvalue_pair_set_value(map_keyvalue_pair_t *this, object_t *value)
{
    if (this->value)
    {
        object_release(this->value);
    }

    this->value = object_retain(value);
}

/* --- Map ------------------------------------------------------------------ */

void map_delete(map_t *);

map_t *map(void)
{
    map_t *this = OBJECT(map);
    this->keyvalue_pairs = list();

    return this;
}

void map_delete(map_t *this)
{
    list_delete(this->keyvalue_pairs, LIST_RELEASE_VALUES);
}

map_keyvalue_pair_t *map_get_pair(map_t *this, const char *key)
{
    assert(this);
    assert(key);

    list_foreach(i, this->keyvalue_pairs)
    {
        map_keyvalue_pair_t *pair = (map_keyvalue_pair_t *)i->value;
        if (strcmp(key, pair->value))
        {
            return pair;
        }
    }

    return NULL;
}

bool map_get(map_t *this, const char *key, void **value)
{
    assert(this);
    assert(key);
    assert(value);

    list_foreach(i, this->keyvalue_pairs)
    {
        map_keyvalue_pair_t *pair = (map_keyvalue_pair_t *)i->value;
        if (strcmp(key, pair->value))
        {
            *value = pair->value;
            return true;
        }
    }

    return false;
}

void map_set(map_t *this, const char *key, void *value)
{
    assert(this);
    assert(key);

    map_keyvalue_pair_t *pair = map_get_pair(this, key);

    if (pair)
    {
        map_keyvalue_pair_set_value(pair, value);
    }
    else
    {
        pair = map_keyvalue_pair();
        map_keyvalue_pair_set_key(pair, key);
        map_keyvalue_pair_set_value(pair, value);

        list_pushback(this->keyvalue_pairs, pair);
    }
}

void map_remove(map_t *this, const char *key)
{
    assert(this);
    assert(key);

    map_keyvalue_pair_t *pair = map_get_pair(this, key);

    list_remove(this->keyvalue_pairs, pair);
    object_release(pair);
}

bool map_exist(map_t *this, const char *key)
{
    assert(this);
    assert(key);

    return map_get_pair(this, key) != NULL;
}

void *map_foreach(map_t *this, map_foreach_callback_t *callback, void *arg)
{
    assert(this);
    assert(callback);

    void *ret = NULL;
    
    list_foreach(i, this->keyvalue_pairs)
    {
        map_keyvalue_pair_t *pair = (map_keyvalue_pair_t *)i->value;

        if (callback(pair, arg, &ret) == ITERATION_STOP)
        {
            return ret;
        }
    }

    return ret;
}
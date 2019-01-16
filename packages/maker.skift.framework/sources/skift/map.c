/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include <skift/map.h>

/*
map_t* map(void)
{
    map_t* map = MALLOC(map_t);

    map->keys = list();
    map->values = list();

    return map;
}

void map_delete(map_t* map)
{
    list_destroy(map->keys);
    list_delete(map->values);
    free(map);
}

void map_destroy(map_t* map)
{
    list_destroy(map->keys);
    list_destroy(map->values);
    free(map);
}

void map_put(map_t* map, const char * key, void * value)
{

}

void map_puti(map_t* map, const char * key, int value)
{
    
}

void * map_get(map_t* map, const char * key)
{

}

int map_geti(map_t* map, const char * key, int default_)
{
    return default_;
}

bool map_exist(map_t* map, const char * key)
{
    FOREACH(k, map->keys)
    {
        if (!strcmp(k->value, key)) return true;
    }

    return false;
}

bool map_containe(map_t* map, const void* value)
{
    FOREACH(v, map->values)
    {
        if (v->value == value) return true;
    }

    return false;
}

bool map_containei(map_t* map, int value)
{
    FOREACH(v, map->values)
    {
        if ((int)v->value == value) return true;
    }

    return false;
}
*/
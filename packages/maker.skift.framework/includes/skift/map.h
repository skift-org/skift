#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/list.h>

typedef struct 
{
    list_t* keys;
    list_t* values;
} map_t;

map_t* map(void);
void map_delete(map_t* map);
void map_destroy(map_t* map);

void map_put(map_t* map, const char * key, void * value);
void map_puti(map_t* map, const char * key, int value);

void * map_get(map_t* map, const char * key);
int map_geti(map_t* map, const char * key, int default_);

bool map_exist(map_t* map, const char * key);
bool map_containe(map_t* map, const void* value);
bool map_containei(map_t* map, int value);
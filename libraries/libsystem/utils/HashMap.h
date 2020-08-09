#pragma once

#include <libsystem/utils/List.h>

typedef void (*HashMapDestroyValueCallback)(void *value);

struct HashMapItem;

struct HashMap;

HashMap *hashmap_create_string_to_value();

void hashmap_destroy(HashMap *hashmap);

void hashmap_destroy_with_callback(HashMap *hashmap, HashMapDestroyValueCallback callback);

void hashmap_clear(HashMap *hashmap);

void hashmap_clear_with_callback(HashMap *hashmap, HashMapDestroyValueCallback callback);

bool hashmap_put(HashMap *hashmap, const void *key, void *value);

void *hashmap_get(HashMap *hashmap, const void *key);

bool hashmap_has(HashMap *hashmap, const void *key);

bool hashmap_remove(HashMap *hashmap, const void *key);

void hashmap_remove_value(HashMap *hashmap, void *value);

bool hashmap_remove_with_callback(
    HashMap *hashmap,
    const void *key,
    HashMapDestroyValueCallback callback);

typedef Iteration (*HashMapIterationCallback)(void *target, void *key, void *value);
bool hashmap_iterate(HashMap *hashmap, void *target, HashMapIterationCallback callback);

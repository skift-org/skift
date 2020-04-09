#pragma once

#include <libsystem/utils/List.h>

typedef void *(*HashMapCopyKeyCallback)(void *value);
typedef uint32_t (*HashMapHashKeyCallback)(void *value);
typedef int (*HashMapCompareKeyCallback)(void *lhs, void *rhs);
typedef void (*HashMapDestroyKeyCallback)(void *value);

typedef void (*HashMapDestroyValueCallback)(void *value);

typedef struct HashMapItem
{
    uint32_t hash;
    void *key;
    void *value;
} HashMapItem;

typedef struct
{
    List **buckets;
    size_t buckets_count;

    HashMapCopyKeyCallback copy_key;
    HashMapHashKeyCallback hash_key;
    HashMapCompareKeyCallback compare_key;
    HashMapDestroyKeyCallback destroy_key;
} HashMap;

HashMap *hashmap_create_string_to_value(void);

void hashmap_destroy(HashMap *hashmap);

void hashmap_destroy_with_callback(HashMap *hashmap, HashMapDestroyValueCallback callback);

void hashmap_clear(HashMap *hashmap);

void hashmap_clear_with_callback(HashMap *hashmap, HashMapDestroyValueCallback callback);

bool hashmap_put(HashMap *hashmap, void *key, void *value);

void *hashmap_get(HashMap *hashmap, void *key);

bool hashmap_has(HashMap *hashmap, void *key);

bool hashmap_remove(HashMap *hashmap, void *key);

bool hashmap_remove_with_callback(
    HashMap *hashmap,
    void *key,
    HashMapDestroyValueCallback callback);

typedef IterationDecision (*HashMapIterationCallback)(void *target, void *key, void *value);
bool hashmap_iterate(HashMap *hashmap, void *target, HashMapIterationCallback callback);

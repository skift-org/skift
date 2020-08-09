#include <libsystem/core/CString.h>
#include <libsystem/utils/HashMap.h>

typedef void *(*HashMapCopyKeyCallback)(const void *value);
typedef uint32_t (*HashMapHashKeyCallback)(const void *value);
typedef int (*HashMapCompareKeyCallback)(const void *lhs, const void *rhs);
typedef void (*HashMapDestroyKeyCallback)(void *value);

struct HashMapItem
{
    uint32_t hash;
    void *key;
    void *value;
};

struct HashMap
{
    List **buckets;
    size_t buckets_count;

    HashMapCopyKeyCallback copy_key;
    HashMapHashKeyCallback hash_key;
    HashMapCompareKeyCallback compare_key;
    HashMapDestroyKeyCallback destroy_key;
};

static char *hashmap_string_copy(const char *string)
{
    return strdup(string);
}

static uint32_t hashmap_string_hash(const char *string)
{
    uint32_t hash = 5381;
    int c;

    while ((c = *string++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

static int hashmap_string_compare(const char *left, const char *right)
{
    return strcmp(left, right);
}

static void hashmap_string_destroy(char *string)
{
    free(string);
}

#define HASHMAP_BUCKET_COUNT 32

HashMap *hashmap_create_string_to_value()
{
    HashMap *hashmap = __create(HashMap);

    hashmap->buckets = (List **)calloc(HASHMAP_BUCKET_COUNT, sizeof(List *));
    hashmap->buckets_count = HASHMAP_BUCKET_COUNT;

    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++)
    {
        hashmap->buckets[i] = list_create();
    }

    hashmap->copy_key = (HashMapCopyKeyCallback)hashmap_string_copy;
    hashmap->hash_key = (HashMapHashKeyCallback)hashmap_string_hash;
    hashmap->compare_key = (HashMapCompareKeyCallback)hashmap_string_compare;
    hashmap->destroy_key = (HashMapDestroyKeyCallback)hashmap_string_destroy;

    return hashmap;
}

void hashmap_destroy(HashMap *hashmap)
{
    hashmap_destroy_with_callback(hashmap, nullptr);
}

void hashmap_destroy_with_callback(
    HashMap *hashmap,
    HashMapDestroyValueCallback callback)
{
    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++)
    {
        list_foreach(HashMapItem, item, hashmap->buckets[i])
        {
            hashmap->destroy_key(item->key);

            if (callback)
            {
                callback(item->value);
            }
        }

        list_destroy_with_callback(hashmap->buckets[i], free);
    }

    free(hashmap->buckets);
    free(hashmap);
}

void hashmap_clear(HashMap *hashmap)
{
    hashmap_clear_with_callback(hashmap, nullptr);
}

void hashmap_clear_with_callback(
    HashMap *hashmap,
    HashMapDestroyValueCallback callback)
{
    for (size_t i = 0; i < HASHMAP_BUCKET_COUNT; i++)
    {
        list_foreach(HashMapItem, item, hashmap->buckets[i])
        {
            hashmap->destroy_key(item->key);

            if (callback)
            {
                callback(item->value);
            }
        }

        list_clear_with_callback(hashmap->buckets[i], free);
    }
}

static HashMapItem *hashmap_find_item(HashMap *hashmap, const void *key)
{
    uint32_t hash = hashmap->hash_key(key);

    List *bucket = hashmap->buckets[hash % hashmap->buckets_count];

    list_foreach(HashMapItem, item, bucket)
    {
        if (item->hash == hash &&
            hashmap->compare_key(item->key, key) == 0)
        {
            return item;
        }
    }

    return nullptr;
}

static HashMapItem *hashmap_find_item_by_value(HashMap *hashmap, void *value)
{
    for (size_t i = 0; i < hashmap->buckets_count; i++)
    {
        List *bucket = hashmap->buckets[i];

        list_foreach(HashMapItem, item, bucket)
        {
            if (item->value == value)
            {
                return item;
            }
        }
    }

    return nullptr;
}

bool hashmap_put(HashMap *hashmap, const void *key, void *value)
{
    if (hashmap_has(hashmap, key))
    {
        return false;
    }

    HashMapItem *item = __create(HashMapItem);

    item->hash = hashmap->hash_key(key);
    item->key = hashmap->copy_key(key);
    item->value = value;

    List *bucket = hashmap->buckets[item->hash % hashmap->buckets_count];

    list_pushback(bucket, item);

    return true;
}

void *hashmap_get(HashMap *hashmap, const void *key)
{
    HashMapItem *item = hashmap_find_item(hashmap, key);

    if (!item)
        return nullptr;

    return item->value;
}

bool hashmap_has(HashMap *hashmap, const void *key)
{
    return hashmap_find_item(hashmap, key) != nullptr;
}

bool hashmap_remove(HashMap *hashmap, const void *key)
{
    return hashmap_remove_with_callback(hashmap, key, nullptr);
}

void hashmap_remove_value(HashMap *hashmap, void *value)
{
    HashMapItem *item = hashmap_find_item_by_value(hashmap, value);

    if (item)
    {
        hashmap_remove(hashmap, item->key);
    }
}

bool hashmap_remove_with_callback(
    HashMap *hashmap,
    const void *key,
    HashMapDestroyValueCallback callback)
{
    HashMapItem *item = hashmap_find_item(hashmap, key);

    if (!item)
    {
        return false;
    }

    uint32_t hash = hashmap->hash_key(key);
    List *bucket = hashmap->buckets[hash % hashmap->buckets_count];

    if (callback)
    {
        callback(item->value);
    }

    hashmap->destroy_key(item->key);

    list_remove_with_callback(bucket, item, free);

    return true;
}

bool hashmap_iterate(HashMap *hashmap, void *target, HashMapIterationCallback callback)
{
    for (size_t i = 0; i < hashmap->buckets_count; i++)
    {
        list_foreach(HashMapItem, item, hashmap->buckets[i])
        {
            if (callback(target, item->key, item->value) == Iteration::STOP)
            {
                return false;
            }
        }
    }

    return true;
}

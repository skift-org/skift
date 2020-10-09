#pragma once

#include <libsystem/Logger.h>
#include <libutils/Hash.h>
#include <libutils/Vector.h>

template <typename TKey, typename TValue>
class HashMap
{
private:
    struct Item
    {
        uint32_t hash;
        TKey key;
        TValue value;
    };

    static constexpr int BUCKET_COUNT = 256;

    Vector<Vector<Item>> _buckets{};

    Vector<Item> &bucket(uint32_t hash)
    {
        return _buckets[hash % BUCKET_COUNT];
    }

    Item *item_by_key(const TKey &key)
    {
        return item_by_key(key, hash<TKey>(key));
    }

    Item *item_by_key(const TKey &key, uint32_t hash)
    {
        Item *result = nullptr;
        auto &b = bucket(hash);

        b.foreach ([&](Item &item) {
            if (item.hash == hash && item.key == key)
            {
                result = &item;
                return Iteration::STOP;
            }
            else
            {
                return Iteration::CONTINUE;
            }
        });

        return result;
    }

public:
    size_t count()
    {
        size_t result = 0;

        for (size_t i = 0; i < BUCKET_COUNT; i++)
        {
            result += _buckets[i].count();
        }

        return result;
    }

    HashMap()
    {
        for (size_t i = 0; i < BUCKET_COUNT; i++)
        {
            _buckets.push_back({});
        }
    }

    void clear()
    {
        _buckets.clear();
    }

    void remove_key(TKey &key)
    {
        uint32_t h = hash<TKey>(key);

        bucket(h).remove_all_match([&](auto &item) {
            return item.hash == h && item.key == key;
        });
    }

    void remove_value(TValue &value)
    {
        _buckets.foreach ([&](auto &bucket) {
            bucket.remove_all_match([&](auto &item) {
                return item.value == value;
            });

            return Iteration::CONTINUE;
        });
    }

    bool has_key(const TKey &key)
    {
        return item_by_key(key) != nullptr;
    }

    bool has_value(const TValue &value)
    {
        bool result = false;

        this->foreach ([&](auto &, auto &v) {
            if (v == value)
            {
                result = true;
                return Iteration::STOP;
            }
            else
            {
                return Iteration::CONTINUE;
            }
        });

        return result;
    }

    template <typename TCallback>
    void foreach (TCallback callback)
    {
        _buckets.foreach ([&](auto &bucket) {
            return bucket.foreach ([&](auto &item) {
                return callback(item.key, item.value);
            });
        });
    }

    TValue &operator[](const TKey &key)
    {
        auto h = hash<TKey>(key);
        auto *i = item_by_key(key, h);

        if (i)
        {
            return i->value;
        }
        else
        {
            auto &b = bucket(h);
            return b.push_back({h, key, {}}).value;
        }
    }
};

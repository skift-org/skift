#pragma once

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
    size_t count() const
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

    HashMap(const HashMap &other)
        : _buckets(other._buckets)
    {
    }

    HashMap(HashMap &&other)
        : _buckets(move(other._buckets))
    {
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
    Iteration foreach (TCallback callback) const
    {
        return _buckets.foreach ([&](auto &bucket) {
            return bucket.foreach ([&](auto &item) {
                return callback(item.key, item.value);
            });
        });
    }

    HashMap &operator=(const HashMap &other)
    {
        _buckets = other._buckets;
        return *this;
    }

    HashMap &operator=(HashMap &&other)
    {
        swap(_buckets, other._buckets);
        return *this;
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

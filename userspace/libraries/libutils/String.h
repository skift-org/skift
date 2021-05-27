#pragma once

#include <libutils/Hash.h>
#include <libutils/RefPtr.h>
#include <libutils/Slice.h>
#include <libutils/Std.h>
#include <libutils/StringStorage.h>

namespace Utils
{

struct String :
    public RawStorage
{
private:
    RefPtr<StringStorage> _storage;

public:
    size_t length() const
    {
        return _storage->size();
    }

    bool empty() const
    {
        return length() == 0;
    }

    const char *cstring() const
    {
        if (!_storage)
        {
            return "";
        }

        return _storage->cstring();
    }

    const char &at(int index) const
    {
        return _storage->cstring()[index];
    }

    bool null_or_empty() const
    {
        return _storage == nullptr || _storage->size() == 0;
    }

    Slice slice() const
    {
        return Slice{_storage};
    }

    Slice slice(size_t start, size_t length) const
    {
        assert(start < this->length());
        assert(start + length <= this->length());

        return Slice{_storage, start, length};
    }

    String(const char *cstring = "")
    {
        _storage = make<StringStorage>(COPY, cstring);
    }

    String(const char *cstring, size_t length)
    {
        _storage = make<StringStorage>(COPY, cstring, length);
    }

    String(char c)
    {
        char cstr[2];
        cstr[0] = c;
        _storage = make<StringStorage>(COPY, cstr, 1);
    }

    String(RefPtr<StringStorage> storage)
        : _storage(storage)
    {
    }

    String(const String &other)
        : _storage(const_cast<String &>(other)._storage)
    {
    }

    String(String &&other)
        : _storage(std::move(other._storage))
    {
    }

    String &operator=(const String &other)
    {
        if (this != &other)
        {
            _storage = const_cast<String &>(other)._storage;
        }

        return *this;
    }

    String &operator=(String &&other)
    {
        if (this != &other)
        {
            std::swap(_storage, other._storage);
        }

        return *this;
    }

    bool operator!=(const String &other) const
    {
        return !(*this == other);
    }

    bool operator==(const String &other) const
    {
        if (_storage == other._storage)
        {
            return true;
        }

        if (length() != other.length())
        {
            return false;
        }

        for (size_t i = 0; i < length(); i++)
        {
            if (cstring()[i] != other.cstring()[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator==(const char *str) const
    {
        if (length() != strlen(str))
        {
            return false;
        }

        for (size_t i = 0; i < length(); i++)
        {
            if (cstring()[i] != str[i])
            {
                return false;
            }
        }

        return true;
    }

    char operator[](int index) const
    {
        return at(index);
    }

    RefPtr<Storage> storage() override
    {
        return _storage;
    }

    RefPtr<StringStorage> string_storage()
    {
        return _storage;
    }
};

template <typename T>
concept ToString = requires(const T &t) { t.string(); };

template <>
inline uint32_t hash<String>(const String &value)
{
    return hash(value.cstring(), value.length());
}

} // namespace Utils
#pragma once

#include <libutils/Hash.h>
#include <libutils/Move.h>
#include <libutils/RefPtr.h>
#include <libutils/StringStorage.h>

#include <libsystem/io_new/Format.h>
#include <libsystem/io_new/MemoryWriter.h>

class String
{
private:
    RefPtr<StringStorage> _buffer;

public:
    template <System::Formatable... Args>
    static String format(const char *fmt, Args... args)
    {
        StringScanner scan{fmt, strlen(fmt)};
        System::MemoryWriter memory;

        System::format(memory, scan, forward<Args>(args)...);

        return memory.string();
    }

    size_t length() const
    {
        return _buffer->length();
    }

    const char *cstring() const
    {
        return _buffer->cstring();
    }

    const char &at(int index) const
    {
        return _buffer->cstring()[index];
    }

    bool null_or_empty() const
    {
        return _buffer == nullptr || _buffer->length() == 0;
    }

    String sub_string(size_t start, size_t length)
    {
        assert(start < this->length());
        assert(start + length <= this->length());

        return make<StringStorage>(&at(start), length);
    }

    String(const char *cstring = "")
    {
        _buffer = make<StringStorage>(cstring);
    }

    String(const char *cstring, size_t length)
    {
        _buffer = make<StringStorage>(cstring, length);
    }

    String(char c)
    {
        char cstr[2];
        cstr[0] = c;
        _buffer = make<StringStorage>(cstr, 1);
    }

    String(RefPtr<StringStorage> storage)
        : _buffer(storage)
    {
    }

    String(const String &other)
        : _buffer(const_cast<String &>(other)._buffer)
    {
    }

    String(String &&other)
        : _buffer(move(other._buffer))
    {
    }

    String &operator=(const String &other)
    {
        if (this != &other)
        {
            _buffer = const_cast<String &>(other)._buffer;
        }

        return *this;
    }

    String &operator=(String &&other)
    {
        if (this != &other)
        {
            swap(_buffer, other._buffer);
        }

        return *this;
    }

    String &operator+=(String &other)
    {
        _buffer = make<StringStorage>(*_buffer, *other._buffer);

        return *this;
    }

    bool operator!=(const String &other) const
    {
        return !(*this == other);
    }

    bool operator==(const String &other) const
    {
        if (_buffer == other._buffer)
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

    RefPtr<StringStorage> underlying_storage()
    {
        return _buffer;
    }
};

template <>
inline uint32_t hash<String>(const String &value)
{
    return hash(value.cstring(), value.length());
}

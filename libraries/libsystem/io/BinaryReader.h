#pragma once

#include <libio/Seek.h>
#include <libsystem/io/SeekableReader.h>
#include <libutils/Slice.h>
#include <libutils/String.h>

class BinaryReader : public SeekableReader
{
public:
    BinaryReader(SeekableReader &reader) : _reader(reader)
    {
    }

    // Peek & Get functons
    template <typename T>
    inline T peek()
    {
        T result = get<T>();
        _reader.seek(IO::SeekFrom::current(-sizeof(T)));
        return result;
    }

    template <typename T>
    inline T get()
    {
        T result;
        assert(_reader.read(&result, sizeof(T)) == sizeof(T));
        return result;
    }

    inline String get_fixed_len_string(size_t len)
    {
        char *cstr = new char[len];
        assert(_reader.read(cstr, len) == len);
        return String(make<StringStorage>(ADOPT, cstr, len));
    }

    inline void skip(size_t num_bytes)
    {
        _reader.seek(IO::SeekFrom::current(num_bytes));
    }

    inline bool good()
    {
        return _reader.position() < _reader.length();
    }

    // Inherited from SeekableReader
    virtual size_t length() override;
    virtual size_t position() override;
    virtual size_t seek(IO::SeekFrom from) override;
    virtual size_t read(void *buffer, size_t size) override;

private:
    SeekableReader &_reader;
};
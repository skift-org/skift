#pragma once
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
        _reader.seek(-sizeof(T), WHENCE_HERE);
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
        return String(make<StringStorage>(cstr, len));
    }

    inline void skip(size_t num_bytes)
    {
        _reader.seek(num_bytes, WHENCE_HERE);
    }

    inline bool good() 
    {
        return _reader.position() < _reader.length();
    }

    // Inherited from SeekableReader
    virtual size_t length() override;
    virtual size_t position() override;
    virtual size_t seek(size_t pos, Whence whence) override;
    virtual size_t read(void *buffer, size_t size) override;
private:
    SeekableReader &_reader;
};
#pragma once

#include <assert.h>
#include <libsystem/Logger.h>
#include <libsystem/io/Stream.h>
#include <libutils/unicode/Codepoint.h>
#include <libutils/NumberParser.h>
#include <libutils/RingBuffer.h>
#include <string.h>

class Scanner
{
public:
    virtual ~Scanner(){};

    virtual bool ended() = 0;

    virtual void foreward() = 0;

    virtual char peek(size_t peek) = 0;

    bool do_continue()
    {
        return !ended();
    }

    void foreward(size_t n)
    {
        for (size_t i = 0; i < n; i++)
        {
            foreward();
        }
    }

    void foreward_codepoint()
    {
        if ((current() & 0xf8) == 0xf0)
        {
            foreward(4);
        }
        else if ((current() & 0xf0) == 0xe0)
        {
            foreward(3);
        }
        else if ((current() & 0xe0) == 0xc0)
        {
            foreward(2);
        }
        else
        {
            foreward(1);
        }
    }

    char current()
    {
        return peek(0);
    }

    Codepoint current_codepoint()
    {
        size_t size = 0;
        Codepoint codepoint = peek(0);

        if ((current() & 0xf8) == 0xf0)
        {
            size = 4;
            codepoint = (0x07 & codepoint) << 18;
        }
        else if ((current() & 0xf0) == 0xe0)
        {
            size = 3;
            codepoint = (0x0f & codepoint) << 12;
        }
        else if ((current() & 0xe0) == 0xc0)
        {
            codepoint = (0x1f & codepoint) << 6;
            size = 2;
        }

        for (size_t i = 1; i < size; i++)
        {
            codepoint |= (0x3f & peek(i)) << (6 * (size - i - 1));
        }

        return codepoint;
    }

    bool current_is(const char *what)
    {
        return current_is(what, strlen(what));
    }

    bool current_is(const char *what, size_t size)
    {
        for (size_t i = 0; i < size; i++)
        {
            if (current() == what[i])
            {
                return true;
            }
        }

        return false;
    }

    bool current_is_word(const char *word)
    {
        for (size_t i = 0; word[i]; i++)
        {
            if (peek(i) != word[i])
            {
                return false;
            }
        }

        return true;
    }

    void eat(const char *what)
    {
        while (current_is(what) &&
               do_continue())
        {
            foreward();
        }
    }

    bool skip(char chr)
    {
        if (current() == chr)
        {
            foreward();

            return true;
        }

        return false;
    }

    bool skip(const char *chr)
    {
        if (current_is(chr))
        {
            foreward();

            return true;
        }

        return false;
    }

    bool skip_word(const char *word)
    {
        if (current_is_word(word))
        {
            for (size_t i = 0; i < strlen(word); i++)
            {
                foreward();
            }

            return true;
        }

        return false;
    }
};

class StreamScanner : public Scanner
{
private:
    static constexpr int PEEK_SIZE = 16;

    RingBuffer _peek{PEEK_SIZE};
    Stream *_stream = nullptr;

    void refill()
    {
        char c = stream_getchar(_stream);

        if (!handle_has_error(_stream))
        {
            _peek.put(c);
        }
    }

public:
    StreamScanner(Stream *stream)
    {
        _stream = stream;
    }

    bool ended() override
    {
        return stream_is_end_file(_stream) ||
               handle_has_error(_stream);
    }

    void foreward() override
    {
        if (_peek.empty())
        {
            refill();
        }

        if (!ended())
        {
            _peek.get();
        }
    }

    char peek(size_t peek) override
    {

        while (peek >= _peek.used() && !ended())
        {
            refill();
        }

        if (ended())
        {
            return '\0';
        }

        return _peek.peek(peek);
    }
};

class StringScanner : public Scanner
{
private:
    const char *_string = nullptr;

    size_t _size = 0;
    size_t _offset = 0;

public:
    StringScanner(const char *string, size_t size)
    {
        _string = string;
        _size = size;
    }

    bool ended() override
    {
        return _offset >= _size;
    }

    void foreward() override
    {
        if (!ended())
        {
            _offset++;
        }
    }

    char peek(size_t peek) override
    {
        size_t offset = _offset + peek;

        if (offset >= _size)
        {
            return '\0';
        }

        return _string[offset];
    }
};

#pragma once

#include <libio/Reader.h>
#include <libtext/Rune.h>
#include <libutils/InlineRingBuffer.h>

#include <string.h>

namespace IO
{

struct Scanner final
{
private:
    static constexpr int MAX_PEEK = 64;

    Reader &_reader;
    InlineRingBuffer<uint8_t, MAX_PEEK> _peek{};
    bool _is_end_of_file = false;

    void refill()
    {
        if (_is_end_of_file)
        {
            return;
        }

        char c = 0x69;
        auto read_result = _reader.read(&c, 1);

        if (read_result.unwrap_or(0) == 0)
        {
            _is_end_of_file = true;
        }
        else
        {
            _peek.put(c);
        }
    }

public:
    Scanner(Reader &reader)
        : _reader{reader}
    {
    }

    bool ended()
    {
        if (_peek.empty())
        {
            refill();
        }

        return _is_end_of_file;
    }

    char next()
    {
        if (_peek.empty())
        {
            refill();
        }

        if (ended())
        {
            return '\0';
        }

        return _peek.get();
    }

    void next(size_t n)
    {
        for (size_t i = 0; i < n; i++)
        {
            next();
        }
    }

    char peek(size_t peek = 0)
    {
        while (!ended() && peek >= _peek.used())
        {
            refill();
        }

        if (ended())
        {
            return '\0';
        }

        return _peek.peek(peek);
    }

    bool peek_is_any(const char *what)
    {
        return peek_is_any(0, what, strlen(what));
    }

    bool peek_is_any(const char *what, size_t size)
    {
        return peek_is_any(0, what, size);
    }

    bool peek_is_any(size_t offset, const char *what)
    {
        return peek_is_any(offset, what, strlen(what));
    }

    bool peek_is_any(size_t offset, const char *what, size_t size)
    {
        for (size_t i = 0; i < size; i++)
        {
            if (peek(offset) == what[i])
            {
                return true;
            }
        }

        return false;
    }

    bool peek_is_word(const char *word)
    {
        return peek_is_word(0, word, strlen(word));
    }

    bool peek_is_word(const char *word, size_t size)
    {
        return peek_is_word(0, word, size);
    }

    bool peek_is_word(size_t offset, const char *word)
    {
        return peek_is_word(offset, word, strlen(word));
    }

    bool peek_is_word(size_t offset, const char *word, size_t size)
    {
        for (size_t i = 0; i < size; i++)
        {
            if (peek(offset + i) != word[i])
            {
                return false;
            }
        }

        return true;
    }

    void next_rune()
    {
        if ((peek() & 0xf8) == 0xf0)
        {
            next(4);
        }
        else if ((peek() & 0xf0) == 0xe0)
        {
            next(3);
        }
        else if ((peek() & 0xe0) == 0xc0)
        {
            next(2);
        }
        else
        {
            next(1);
        }
    }

    Text::Rune peek_rune()
    {
        size_t size = 0;
        Text::Rune rune = peek(0);

        if ((peek() & 0xf8) == 0xf0)
        {
            size = 4;
            rune = (0x07 & rune) << 18;
        }
        else if ((peek() & 0xf0) == 0xe0)
        {
            size = 3;
            rune = (0x0f & rune) << 12;
        }
        else if ((peek() & 0xe0) == 0xc0)
        {
            rune = (0x1f & rune) << 6;
            size = 2;
        }

        for (size_t i = 1; i < size; i++)
        {
            rune |= (0x3f & peek(i)) << (6 * (size - i - 1));
        }

        return rune;
    }

    bool skip(char chr)
    {
        return skip_any(&chr, 1);
    }

    void eat_any(const char *what)
    {
        return eat_any(what, strlen(what));
    }

    void eat_any(const char *what, size_t size)
    {
        while (peek_is_any(what, size) && !ended())
        {
            next();
        }
    }

    bool skip_any(const char *chr)
    {
        return skip_any(chr, strlen(chr));
    }

    bool skip_any(const char *chr, size_t size)
    {
        if (peek_is_any(chr, size))
        {
            next();

            return true;
        }

        return false;
    }

    bool skip_word(const char *word)
    {
        return skip_word(word, strlen(word));
    }

    bool skip_word(const char *word, size_t size)
    {
        if (peek_is_word(word))
        {
            for (size_t i = 0; i < size; i++)
            {
                next();
            }

            return true;
        }

        return false;
    }

    void skip_utf8bom()
    {
        skip_word("\xEF\xBB\xBF");
    }
};

} // namespace IO

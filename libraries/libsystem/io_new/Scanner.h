#pragma once

#include <libutils/InlineRingBuffer.h>

#include <libsystem/io_new/Reader.h>

namespace System
{

template <typename TReader>
class Scanner final
{
private:
    static constexpr int PEEK_SIZE = 16;

    TReader _reader;
    utils::InlineRingBuffer<uint8_t, 16> _peek{};
    bool _is_end_of_file = false;

    void refill()
    {
        auto read_result = _reader.read_byte();

        if (!read_result)
        {
            _is_end_of_file = true;
            return;
        }

        _peek.put(*read_result);
    }

public:
    Scanner(TReader reader) : _reader(reader)
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

    void foreward()
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

    char peek(size_t peek)
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

} // namespace System

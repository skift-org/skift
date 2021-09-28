#pragma once

#include <libtext/Rune.h>
#include <libutils/Func.h>

namespace Text
{

struct UTF8Decoder
{
private:
    bool _decoding = false;
    Text::Rune _accumulator = 0;
    int _remaining = 0;

    Func<void(Text::Rune)> _callback{};

public:
    void callback(Func<void(Text::Rune)> callback)
    {
        _callback = callback;
    }

    UTF8Decoder()
    {
    }

    UTF8Decoder(Func<void(Text::Rune)> callback)
        : _callback(callback)
    {
    }

    void write(uint8_t byte)
    {
        if (!_decoding)
        {
            if ((byte & 0xf8) == 0xf0)
            {
                _accumulator = (0x07 & byte) << 18;

                _decoding = true;
                _remaining = 3;
            }
            else if ((byte & 0xf0) == 0xe0)
            {
                _accumulator = (0x0f & byte) << 12;

                _decoding = true;
                _remaining = 2;
            }
            else if ((byte & 0xe0) == 0xc0)
            {
                _accumulator = (0x1f & byte) << 6;

                _decoding = true;
                _remaining = 1;
            }
            else
            {
                _accumulator = byte;

                _decoding = true;
                _remaining = 0;
            }
        }
        else
        {
            if ((byte & 0b11000000) == 0b10000000)
            {
                _remaining--;
                _accumulator |= (0x3f & byte) << (6 * _remaining);
            }
            else
            {
                // FIXME: maybe have a special callback for this case.
                _decoding = false;
                _callback(U'?');

                return;
            }
        }

        if (_decoding && _remaining == 0)
        {
            _decoding = false;
            _callback(_accumulator);

            return;
        }
    }

    void flush()
    {
        _decoding = false;
        _accumulator = 0;
        _remaining = 0;
    }
};

} // namespace Text

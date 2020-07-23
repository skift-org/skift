
#include <libsystem/system/Logger.h>
#include <libsystem/unicode/UTF8Decoder.h>

UTF8Decoder *utf8decoder_create(void *target, UTF8DecoderCallback callback)
{
    UTF8Decoder *decoder = __create(UTF8Decoder);

    decoder->is_decoding = false;
    decoder->current_decoding = 0;
    decoder->width_decoding = 0;

    decoder->target = target;
    decoder->callback = callback;

    return decoder;
}

void utf8decoder_destroy(UTF8Decoder *decoder)
{
    free(decoder);
}

void utf8decoder_write(UTF8Decoder *decoder, uint8_t byte)
{
    if (!decoder->is_decoding)
    {
        if ((byte & 0xf8) == 0xf0)
        {
            decoder->current_decoding = (0x07 & byte) << 18;

            decoder->is_decoding = true;
            decoder->width_decoding = 3;
        }
        else if ((byte & 0xf0) == 0xe0)
        {
            decoder->current_decoding = (0x0f & byte) << 12;

            decoder->is_decoding = true;
            decoder->width_decoding = 2;
        }
        else if ((byte & 0xe0) == 0xc0)
        {
            decoder->current_decoding = (0x1f & byte) << 6;

            decoder->is_decoding = true;
            decoder->width_decoding = 1;
        }
        else
        {
            decoder->current_decoding = byte;

            decoder->is_decoding = true;
            decoder->width_decoding = 0;
        }
    }
    else
    {
        if ((byte & 0b11000000) == 0b10000000)
        {
            decoder->width_decoding--;
            decoder->current_decoding |= (0x3f & byte) << (6 * decoder->width_decoding);
        }
        else
        {
            // FIXME: maybe have a special callback for this case.
            decoder->is_decoding = false;
            decoder->callback(decoder->target, U'?');

            return;
        }
    }

    if (decoder->is_decoding && decoder->width_decoding == 0)
    {
        decoder->is_decoding = false;
        decoder->callback(decoder->target, decoder->current_decoding);

        return;
    }
}

void utf8decoder_flush(UTF8Decoder *decoder)
{
    decoder->is_decoding = false;
    decoder->current_decoding = 0;
    decoder->width_decoding = 0;
}

#include <stdio.h>

#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("Usage: %s FILENAME", argv[0]);
        return PROCESS_FAILURE;
    }

    Stream *stream = stream_open(argv[1], OPEN_READ);

    if (handle_has_error(stream))
    {
        handle_printf_error(stream, "Couldn't read %s", argv[1]);
        return PROCESS_FAILURE;
    }

    size_t read;
    size_t offset = 0;
    uint8_t buffer[16];

    while ((read = stream_read(stream, buffer, 16)) != 0)
    {
        printf("%08x ", (unsigned int)offset * 16);
        for (unsigned char i : buffer)
        {
            printf("%02x ", i);
        }

        printf("\n");
        offset++;

        if (handle_has_error(out_stream))
        {
            handle_printf_error(out_stream, "Couldn't write to stdout\n");
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}

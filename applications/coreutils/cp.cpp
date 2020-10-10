#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>

// implementation of cp open a input stream from file to copied and create a new file and open it as output stream and
// paste all the content into it.
// Add more options
// - Create new file
// - append to already existing file
// - only copy if file already exists
// - only logs if used with option -v

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        // open input stream
        __cleanup(stream_cleanup) Stream *streamin = stream_open(argv[1], OPEN_READ);

        if (handle_has_error(streamin))
        {
            handle_printf_error(streamin, "cp: Cannot access %s", argv[1]);
            return -1;
        }
        // create new file
        Stream *stream = stream_open(argv[2], OPEN_CREATE);
        stream_close(stream);

        // open output stream
        __cleanup(stream_cleanup) Stream *streamout = stream_open(argv[2], OPEN_WRITE);
        if (handle_has_error(streamout))
        {
            handle_printf_error(streamout, "cp: Cannot access %s", argv[2]);
            return -1;
        }

        size_t read;
        //create buffer
        char buffer[1024];

        //read from file1 and put it in file2
        while ((read = stream_read(streamin, &buffer, 1024)) != 0)
        {
            if (handle_has_error(streamin))
            {
                handle_printf_error(streamin, "cp: Failed to read from %s", argv[1]);

                return -1;
            }

            stream_write(streamout, buffer, read);

            if (handle_has_error(streamout))
            {
                handle_printf_error(streamout, "cp: Failed to write to %s", argv[2]);

                return -1;
            }
        }
        //close streams
        stream_close(streamin);
        stream_close(streamout);
    }

    return -1;
}

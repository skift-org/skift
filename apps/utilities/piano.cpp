#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libutils/Path.h>

// piano utility takes a file that has notes data for eg:
// echo "c5d5e5g5a5b5c6" > new.txt
// piano new.txt

struct spkr
{
    int length;
    int frequency;
};

void note(int length, int frequency, Stream *spkrstream)
{
    struct spkr s = {
        .length = length,
        .frequency = frequency,
    };

    stream_write(spkrstream, &s, sizeof(s));
}

int main(int argc, char *argv[])
{

    if (argc == 1)
    {
        stream_format(err_stream, "%s: Missing notes file operand\n Info : Write notes eg \"c5e5g5\" to a file and supply that files' path to this utility \n", argv[0]);
        return PROCESS_FAILURE;
    }

    __cleanup(stream_cleanup) Stream *streamin = stream_open(argv[1], OPEN_READ);

    if (handle_has_error(streamin))
    {
        return handle_get_error(streamin);
    }

    __cleanup(stream_cleanup) Stream *streamout = stream_open("/Devices/speaker", OPEN_WRITE | OPEN_CREATE);

    if (handle_has_error(streamout))
    {
        return handle_get_error(streamout);
    }

    char c;
    char num;
    int read = 0;
    while ((read = stream_read(streamin, &c, 1)) != 0)
    {

        read = stream_read(streamin, &num, 1);
        switch (c)
        {
        case 'c':
            switch (num)
            {
            case '5':
                note(500, 523, streamout);
                break;
            case '6':
                note(500, 1046, streamout);
                break;
            case '7':
                note(500, 2093, streamout);
                break;
            case '8':
                note(500, 4186, streamout);
                break;
            }
            break;
        case 'd':
            switch (num)
            {
            case '5':
                note(500, 587, streamout);
                break;
            case '6':
                note(500, 1175, streamout);
                break;
            case '7':
                note(500, 2349, streamout);
                break;
            }
            break;
        case 'e':
            switch (num)
            {
            case '5':
                note(500, 659, streamout);
                break;
            case '6':
                note(500, 1319, streamout);
                break;
            case '7':
                note(500, 2637, streamout);
                break;
            }
            break;
        case 'f':
            switch (num)
            {
            case '5':
                note(500, 698, streamout);
                break;
            case '6':
                note(500, 1397, streamout);
                break;
            case '7':
                note(500, 2793, streamout);
                break;
            }
            break;
        case 'g':
            switch (num)
            {
            case '5':
                note(500, 784, streamout);
                break;
            case '6':
                note(500, 1568, streamout);
                break;
            case '7':
                note(500, 3136, streamout);
                break;
            }
            break;
        case 'a':
            switch (num)
            {
            case '5':
                note(500, 880, streamout);
                break;
            case '6':
                note(500, 1760, streamout);
                break;
            case '7':
                note(500, 3520, streamout);
                break;
            }
            break;
        case 'b':
            switch (num)
            {
            case '5':
                note(500, 988, streamout);
                break;
            case '6':
                note(500, 1976, streamout);
                break;
            case '7':
                note(500, 3951, streamout);
                break;
            }
            break;
        }
    }

    return 0;
}

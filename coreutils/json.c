#include <libjson/Json.h>
#include <libsystem/io/Stream.h>

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        Stream *json_file = stream_open(argv[1], OPEN_READ);
        FileState json_state = {};
        stream_stat(json_file, &json_state);

        char *buffer = (char *)malloc(json_state.size);
        stream_read(json_file, buffer, json_state.size);

        JsonValue *json_object = json_parse(buffer, json_state.size);

        char *json_string = json_prettify(json_object);

        printf("%s", json_string);
    }

    return 0;
}

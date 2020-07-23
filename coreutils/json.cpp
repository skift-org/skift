#include <libjson/Json.h>
#include <libsystem/io/Stream.h>

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        JsonValue *json_object = json_parse_file(argv[1]);

        char *json_string = json_prettify(json_object);

        printf("%s", json_string);
    }

    return 0;
}

#include <libjson/Json.h>
#include <libsystem/io/Stream.h>

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        auto json_object = json::parse_file(argv[1]);

        auto json_string = json::prettify(json_object);

        printf("%s", json_string);
    }

    return 0;
}

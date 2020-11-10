#include <libsystem/io/Stream.h>
#include <libsystem/json/Json.h>

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        StreamScanner scan{in_stream};

        auto json_object = json::parse(scan);
        auto json_string = json::prettify(json_object);

        printf("%s", json_string.cstring());

        return PROCESS_SUCCESS;
    }
    else
    {
        auto json_object = json::parse_file(argv[1]);
        auto json_string = json::prettify(json_object);

        printf("%s", json_string.cstring());

        return PROCESS_SUCCESS;
    }
}

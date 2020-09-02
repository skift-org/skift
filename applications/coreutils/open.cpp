#include <libjson/Json.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Path.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>

#define FILE_EXTENSIONS_DATABASE_PATH "/System/Configs/open/file-extensions.json"
#define FILE_TYPES_DATABASE_PATH "/System/Configs/open/file-types.json"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        stream_format(err_stream, "Usage: open FILENAME\n");
        return -1;
    }

    Path *path = path_create(argv[1]);

    const char *extension = path_extension(path);

    if (extension == nullptr)
    {
        stream_format(err_stream, "The file does not have an extension.\n");
        return -1;
    }
    auto file_extensions = json::parse_file(FILE_EXTENSIONS_DATABASE_PATH);

    if (!json::is(file_extensions, json::OBJECT))
    {
        stream_format(err_stream, "The file extensions database is not found (" FILE_EXTENSIONS_DATABASE_PATH ").\n");
        return -1;
    }

    auto file_type = json::object_get(file_extensions, extension);

    if (!json::is(file_type, json::STRING))
    {
        stream_format(err_stream, "Unknown file extension %s.\n", extension);
        return -1;
    }

    auto file_types = json::parse_file(FILE_TYPES_DATABASE_PATH);

    if (!json::is(file_types, json::OBJECT))
    {
        stream_format(err_stream, "The file types database is not found (" FILE_TYPES_DATABASE_PATH ").\n");
        return -1;
    }

    auto file_type_info = json::object_get(file_types, json::string_value(file_type));

    if (!json::is(file_type_info, json::OBJECT))
    {
        stream_format(err_stream, "Unknown file type %s.\n", json::string_value(file_type));
        return -1;
    }

    auto file_type_open_with = json::object_get(file_type_info, "open-with");

    if (!json::is(file_type_open_with, json::STRING))
        return -1;

    const char *application_name = json::string_value(file_type_open_with);
    char application_path[PATH_LENGTH] = {};

    snprintf(application_path, PATH_LENGTH, "/Applications/%s/%s", application_name, application_name);

    Launchpad *launchpad = launchpad_create(application_name, application_path);
    launchpad_argument(launchpad, argv[1]);

    if (launchpad_launch(launchpad, nullptr) != SUCCESS)
        return -1;

    return 0;
}

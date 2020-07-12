#include <libjson/Json.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/Path.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>

#define FILE_EXTENSIONS_DATABASE_PATH "/System/Configs/open/file-extensions.json"
#define FILE_TYPES_DATABASE_PATH "/System/Configs/open/file-types.json"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        stream_printf(err_stream, "Usage: open FILENAME\n");
        return -1;
    }

    Path *path = path_create(argv[1]);

    const char *extension = path_extension(path);

    if (extension == NULL)
    {
        stream_printf(err_stream, "The file does not have an extension.\n");
        return -1;
    }
    JsonValue *file_extensions = json_parse_file(FILE_EXTENSIONS_DATABASE_PATH);

    if (!json_is(file_extensions, JSON_OBJECT))
    {
        stream_printf(err_stream, "The file extension database is not found (" FILE_EXTENSIONS_DATABASE_PATH ").\n");
        return -1;
    }

    JsonValue *file_type = json_object_get(file_extensions, extension);

    if (!json_is(file_type, JSON_STRING))
        return -1;

    JsonValue *file_types = json_parse_file(FILE_TYPES_DATABASE_PATH);

    if (!json_is(file_types, JSON_OBJECT))
        return -1;

    JsonValue *file_type_info = json_object_get(file_types, json_string_value(file_type));

    if (!json_is(file_type_info, JSON_OBJECT))
        return -1;

    JsonValue *file_type_open_with = json_object_get(file_type_info, "open-with");

    if (!json_is(file_type_open_with, JSON_STRING))
        return -1;

    const char *application_name = json_string_value(file_type_open_with);
    char application_path[PATH_LENGTH] = {};

    snprintf(application_path, PATH_LENGTH, "/Applications/%s/%s", application_name, application_name);

    Launchpad *launchpad = launchpad_create(application_name, application_path);
    launchpad_argument(launchpad, argv[1]);

    if (launchpad_launch(launchpad, NULL) != SUCCESS)
        return -1;

    return 0;
}

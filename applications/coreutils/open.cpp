#include <libjson/Json.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Path.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>

#define FILE_EXTENSIONS_DATABASE_PATH "/System/Configs/open/file-extensions.json"
#define FILE_TYPES_DATABASE_PATH "/System/Configs/open/file-types.json"

Result open(Path *path)
{
    char *pathstr = path_as_string(path);
    const char *extension = path_extension(path);

    if (extension == nullptr)
    {
        return ERR_EXTENSION;
    }
    auto file_extensions = json::parse_file(FILE_EXTENSIONS_DATABASE_PATH);

    if (!json::is(file_extensions, json::OBJECT))
    {
        stream_format(err_stream, "The file extensions database is not found (" FILE_EXTENSIONS_DATABASE_PATH ").\n");
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    auto file_type = json::object_get(file_extensions, extension);

    if (!json::is(file_type, json::STRING))
    {
        return ERR_EXTENSION;
    }

    auto file_types = json::parse_file(FILE_TYPES_DATABASE_PATH);

    if (!json::is(file_types, json::OBJECT))
    {
        stream_format(err_stream, "The file types database is not found (" FILE_TYPES_DATABASE_PATH ").\n");
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    auto file_type_info = json::object_get(file_types, json::string_value(file_type));

    if (!json::is(file_type_info, json::OBJECT))
    {
        return ERR_EXTENSION;
    }

    auto file_type_open_with = json::object_get(file_type_info, "open-with");

    if (!json::is(file_type_open_with, json::STRING))
    {
        return ERR_EXTENSION;
    }

    const char *application_name = json::string_value(file_type_open_with);
    char application_path[PATH_LENGTH] = {};

    snprintf(application_path, PATH_LENGTH, "/Applications/%s/%s", application_name, application_name);

    Launchpad *launchpad = launchpad_create(application_name, application_path);
    launchpad_argument(launchpad, pathstr);

    Result result = launchpad_launch(launchpad, nullptr);
    free(pathstr);

    return result;
}

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        stream_format(err_stream, "Usage: open FILENAME\n");
        return PROCESS_FAILURE;
    }

    Path *path = path_create(argv[1]);
    Result result = open(path);

    if (result != SUCCESS)
    {
        stream_format(err_stream, "%s: %s: %s", argv[0], argv[1], get_result_description(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}

#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/json/Json.h>
#include <libsystem/process/Launchpad.h>
#include <libutils/Path.h>

#define FILE_EXTENSIONS_DATABASE_PATH "/Configs/open/file-extensions.json"
#define FILE_TYPES_DATABASE_PATH "/Configs/open/file-types.json"

Result open(const char *raw_path)
{
    auto path = Path::parse(raw_path);

    auto extension = path.extension();

    if (extension == "")
    {
        return ERR_EXTENSION;
    }

    auto file_extensions = json::parse_file(FILE_EXTENSIONS_DATABASE_PATH);

    if (!file_extensions.is(json::OBJECT))
    {
        stream_format(err_stream, "The file extensions database is not found (" FILE_EXTENSIONS_DATABASE_PATH ").\n");
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    const auto &file_type = file_extensions.get(extension);

    if (!file_type.is(json::STRING))
    {
        return ERR_EXTENSION;
    }

    auto file_types = json::parse_file(FILE_TYPES_DATABASE_PATH);

    if (!file_types.is(json::OBJECT))
    {
        stream_format(err_stream, "The file types database is not found (" FILE_TYPES_DATABASE_PATH ").\n");
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    auto file_type_info = file_types.get(file_type.as_string());

    if (!file_type_info.is(json::OBJECT))
    {
        return ERR_EXTENSION;
    }

    auto file_type_open_with = file_type_info.get("open-with");

    if (!file_type_open_with.is(json::STRING))
    {
        return ERR_EXTENSION;
    }

    auto app_name = file_type_open_with.as_string();
    char app_path[PATH_LENGTH] = {};

    snprintf(app_path, PATH_LENGTH, "/Applications/%s/%s", app_name.cstring(), app_name.cstring());

    auto *launchpad = launchpad_create(app_name.cstring(), app_path);
    launchpad_argument(launchpad, path.string().cstring());
    return launchpad_launch(launchpad, nullptr);
}

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        stream_format(err_stream, "Usage: open FILENAME\n");
        return PROCESS_FAILURE;
    }

    Result result = open(argv[1]);
    if (result != SUCCESS)
    {
        stream_format(err_stream, "%s: %s: %s\n", argv[0], argv[1], get_result_description(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}

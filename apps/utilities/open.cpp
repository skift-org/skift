#include <libio/Streams.h>
#include <libsystem/process/Launchpad.h>
#include <libutils/Path.h>
#include <libutils/json/Json.h>
#include <libio/File.h>

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

    IO::File database{FILE_EXTENSIONS_DATABASE_PATH, OPEN_READ};
    auto file_extensions = Json::parse(database);

    if (!file_extensions.is(Json::OBJECT))
    {
        IO::errln("The file extensions database is not found (" FILE_EXTENSIONS_DATABASE_PATH ").");
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    const auto &file_type = file_extensions.get(extension);

    if (!file_type.is(Json::STRING))
    {
        return ERR_EXTENSION;
    }

    IO::File type_database{FILE_TYPES_DATABASE_PATH, OPEN_READ};
    auto file_types = Json::parse(type_database);

    if (!file_types.is(Json::OBJECT))
    {
        IO::errln("The file types database is not found (" FILE_TYPES_DATABASE_PATH ").");
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    auto file_type_info = file_types.get(file_type.as_string());

    if (!file_type_info.is(Json::OBJECT))
    {
        return ERR_EXTENSION;
    }

    auto file_type_open_with = file_type_info.get("open-with");

    if (!file_type_open_with.is(Json::STRING))
    {
        return ERR_EXTENSION;
    }

    auto app_name = file_type_open_with.as_string();

    auto app_path = IO::format("/Applications/{}/{}", app_name, app_name);

    auto *launchpad = launchpad_create(app_name.cstring(), app_path.cstring());
    launchpad_argument(launchpad, path.string().cstring());
    return launchpad_launch(launchpad, nullptr);
}

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        IO::errln("Usage: open FILENAME");
        return PROCESS_FAILURE;
    }

    Result result = open(argv[1]);
    if (result != SUCCESS)
    {
        IO::errln("{}: {}: {}", argv[0], argv[1], get_result_description(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}

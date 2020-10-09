#include <libjson/Json.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/Stream.h>
#include <libsystem/utils/BufferBuilder.h>

void list_pages()
{
    const char *manpages = "/System/Manuals/";

    Directory *directory = directory_open(manpages, OPEN_READ);
    DirectoryEntry entry;

    while (directory_read(directory, &entry) > 0)
    {
        auto buf = buffer_builder_create(strlen(manpages) + strlen(entry.name));
        buffer_builder_append_str(buf, manpages);
        buffer_builder_append_str(buf, entry.name);

        const char *buffer = buffer_builder_intermediate(buf);
        Path *p = path_create(buffer);
        if (strcmp(path_extension(p), ".json") == 0)
        {
            String file = path_filename_without_extension(p);
            printf("%s\n", file.cstring());
        }
        path_destroy(p);
        buffer_builder_destroy(buf);
    }
}

int main(int argc, char **argv)
{
    bool man_list = false;

    if (argc == 1)
    {
        printf("What manual page do you need? eg. man man\n");
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--list") == 0 || strcmp(argv[i], "-l") == 0)
        {
            man_list = true;
            break;
        }

        char manpath[PATH_LENGTH];
        snprintf(manpath, PATH_LENGTH, "/System/Manuals/%s%s", argv[i], ".json");

        __cleanup(stream_cleanup) Stream *stream = stream_open(manpath, OPEN_READ);
        if (handle_has_error(stream))
        {
            handle_printf_error(stream, "man: No manual entry for \"%s\"", argv[i]);
            return -1;
        }

        auto root = json::parse_file(manpath);

        if (json::is(root, json::OBJECT))
        {
            const char *message = "man: It looks like this man page is corrupted\n";

            const char *description = "";
            if (json::object_has(root, "description"))
            {
                auto value = json::object_get(root, "description");

                if (json::is(value, json::STRING))
                {
                    description = json::string_value(value);
                }
                else
                {
                    printf(message);
                    return -1;
                }
            }
            else
            {
                printf(message);
                return -1;
            }

            const char *usage = "";
            if (json::object_has(root, "usage"))
            {
                auto value = json::object_get(root, "usage");
                if (json::is(value, json::STRING))
                {
                    usage = json::string_value(value);
                }
                else
                {
                    printf(message);
                    return -1;
                }
            }
            else
            {
                printf(message);
                return -1;
            }

            const char *example = "";
            if (json::object_has(root, "example"))
            {
                auto value = json::object_get(root, "example");
                if (json::is(value, json::STRING))
                {
                    example = json::string_value(value);
                }
                else
                {
                    printf(message);
                    return -1;
                }
            }
            else
            {
                printf(message);
                return -1;
            }

            printf("%s -\nDESCRIPTION:\n %s\nUSAGE:\n%s\nEXAMPLE:\n%s\n", argv[i], description, usage, example);
        }
        else
        {
            printf("man: No manual entry for \"%s\"\n", argv[i]);
        }
    }

    if (man_list)
    {
        list_pages();
    }

    return 0;
}

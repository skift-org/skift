#include <stdio.h>
#include <libutils/String.h>
#include <libjson/Json.h>
#include <libsystem/io/Stream.h>
#include <libsystem/io/Directory.h>
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
		// This is why c code can be annoying sometimes
		char tmp[strlen("/System/Manuals/") + strlen(argv[i]) + strlen(".json")];
		sprintf(tmp, "/System/Manuals/%s%s", argv[i], ".json");
		const char *manpath = tmp;
		__cleanup(stream_cleanup) Stream *stream = stream_open(manpath, OPEN_READ);
		if (handle_has_error(stream))
		{
			handle_printf_error(stream, "man: No manual entry for \"%s\"", argv[i]);
			return -1;
		}
		JsonValue *root = json_parse_file(manpath);

		if (json_is(root, JSON_OBJECT))
		{
			const char *message = "man: It looks like this man page is corrupted\n";

			const char *description = "";
			const char *usage = "";
			const char *example = "";

			if (json_object_has(root, "description"))
			{
				JsonValue *value = json_object_get(root, "description");
				if (json_is(value, JSON_STRING))
				{
					description = json_string_value(value);
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

			if (json_object_has(root, "usage"))
			{
				JsonValue *value = json_object_get(root, "usage");
				if (json_is(value, JSON_STRING))
				{
					usage = json_string_value(value);
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

			if (json_object_has(root, "example"))
			{
				JsonValue *value = json_object_get(root, "example");
				if (json_is(value, JSON_STRING))
				{
					example = json_string_value(value);
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

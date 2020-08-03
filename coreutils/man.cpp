#include <libjson/Json.h>
#include <stdio.h>
#include <string.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{

	if (argc == 1)
	{
		printf("What manual page do you need? eg. man man\n");
	}
	for (int i = 1; i < argc; i++)
	{
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
return 0;
}

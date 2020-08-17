# Reading JSON files

This example shows how to read JSON files

**file.json**
```json
{
	"value": "hello",
	"object": {
			"obj_value": "object value"
	}
}

```

**main.cpp**
```c++
#include <libjson/Json.h>
#include <stdio.h>
#include <string.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
	// unused parameters
	__unused(argc);
	__unused(argv);

	const char *file = "/file.json";
	__cleanup(stream_cleanup) Stream *stream = stream_open(file, OPEN_READ);
	// Checks if the file can be read
	if (handle_has_error(stream))
	{
		handle_printf_error(stream, "Json file \"%s\" couldn't be read or is not found", file);
		return -1;
	}
	auto root = json::parse_file(file);

	if (json::is(root, json::OBJECT))
	{
		const char *value = "";
		const char *obj_value = "";

		// Checks if the json file has the value object
		if (json::object_has(root, "value"))
		{
			auto value_json = json::object_get(root, "value");
			if (json::is(value_json, json::STRING))
			{
				value = json::string_value(value_json);
			}
			else
			{
				printf("Invalid JSON");
				return -1;
			}
		}
		else
		{
			printf("value in json file not found");
			return -1;
		}

		if (json::object_has(root, "object"))
		{
			auto object_json = json::object_get(root, "object");
			if (json::is(object_json, json::OBJECT))
			{
				auto obj_value_json = json::object_get(object_json, "obj_value");
				if (json::is(obj_value_json, json::STRING))
				{
					obj_value = json::string_value(obj_value_json);
				}
			}
			else
			{
				printf("object is not an json::OBJECT");
				return -1;
			}
		}
		else
		{
			printf("object not found in JSON file");
			return -1;
		}

		printf("%s\n%s\n", value, obj_value);
	}
	else
	{
		printf("Couldn't find JSON file");
	}

	return 0;
}

```

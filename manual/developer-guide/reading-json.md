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
	JsonValue *root = json_parse_file(file);

	if (json_is(root, JSON_OBJECT))
	{
		const char *value = "";
		const char *obj_value = "";

		// Checks if the json file has the value object
		if (json_object_has(root, "value"))
		{
			JsonValue *value_json = json_object_get(root, "value");
			if (json_is(value_json, JSON_STRING))
			{
				value = json_string_value(value_json);
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

		if (json_object_has(root, "object"))
		{
			JsonValue *object_json = json_object_get(root, "object");
			if (json_is(object_json, JSON_OBJECT))
			{
				JsonValue *obj_value_json = json_object_get(object_json, "obj_value");
				if (json_is(obj_value_json, JSON_STRING))
				{
					obj_value = json_string_value(obj_value_json);
				}
			}
			else
			{
				printf("object is not an JSON_OBJECT");
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
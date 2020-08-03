#include <libjson/Json.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    const char* manpath = "/System/Configs/man-pages.json";
    __cleanup(stream_cleanup) Stream *stream = stream_open(manpath, OPEN_READ);
    if (handle_has_error(stream))
    {
        handle_printf_error(stream, "man: Cannot access %s", manpath);
        return -1;
    }
    JsonValue *root = json_parse_file(manpath);

    if(argc == 1)
    {
        printf("What manual page do you need? eg. man man\n");
    }
    for (int i = 1; i < argc; i++)
    {
        if (json_is(root, JSON_OBJECT))
    	{
           if (json_object_has(root, argv[i]))
           {
	       JsonValue *manpage = json_object_get(root, argv[i]);
	       const char* message = "man: It looks like your %s file is corrupted\n";

	       const char* description = "";
	       const char* usage = "";
	       const char* example = "";
	       if (json_object_has(manpage, "description")) { JsonValue *value = json_object_get(manpage, "description"); if(json_is(value, JSON_STRING)) { description = json_string_value(value); } else { printf(message, manpath); return -1; }  } else { printf(message, manpath); return -1; }
	       if (json_object_has(manpage, "usage")) { JsonValue *value = json_object_get(manpage, "usage"); if(json_is(value, JSON_STRING)) { usage = json_string_value(value); } else { printf(message, manpath); return -1; }  } else { printf(message, manpath); return -1; }
	       if (json_object_has(manpage, "example")) { JsonValue *value = json_object_get(manpage, "example"); if(json_is(value, JSON_STRING)) { example = json_string_value(value); } else { printf(message, manpath); return -1; }  } else { printf(message, manpath); return -1; }
               printf("%s -\nDESCRIPTION:\n %s\nUSAGE:\n%s\nEXAMPLE:\n%s\n", argv[i], description, usage, example);
           } else {
               printf("man: No manual entry for \"%s\"\n", argv[i]);
	   }
        }
    }
    return 0;
}

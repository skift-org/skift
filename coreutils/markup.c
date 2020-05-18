#include <libmarkup/Markup.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        MarkupNode *markup_node = markup_parse_file(argv[1]);

        __cleanup_malloc char *markup_string = markup_prettify(markup_node);

        //printf("length: %d\n", strlen(markup_string));
        printf("%s", markup_string);
    }

    return 0;
}
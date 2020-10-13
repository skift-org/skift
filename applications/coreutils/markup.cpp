#include <libmarkup/Markup.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        stream_format(err_stream, "Usage: %s FILENAME\n", argv[0]);
        return PROCESS_FAILURE;
    }

    MarkupNode *markup_node = markup_parse_file(argv[1]);
    __cleanup_malloc char *markup_string = markup_prettify(markup_node);

    printf("%s", markup_string);

    return PROCESS_SUCCESS;
}

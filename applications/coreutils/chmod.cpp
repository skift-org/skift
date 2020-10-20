#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libsystem/utils/NumberParser.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: missing operand", argv[0]);
        return PROCESS_FAILURE;
    }

    if (argc < 3)
    {
        stream_format(err_stream, "%s: extra operand\n,", argv[2]);
        return PROCESS_FAILURE;
    }

    uint mode = 0;

    char u_str_c[] = {argv[1][0], 0};
    char g_str_c[] {argv[1][1], 0};
    char o_str_c[] = {argv[1][2], 0};

    char *u_str = &u_str_c[0];
    char *g_str = &g_str_c[0];
    char *o_str = &o_str_c[0];

    mode |= parse_uint_inline(PARSER_OCTAL, u_str, -1) << 6;
    mode |= parse_uint_inline(PARSER_OCTAL, g_str, -1) << 3;
    mode |= parse_uint_inline(PARSER_OCTAL, o_str, -1) << 0;

    return filesystem_chmod(argv[1], mode);
}

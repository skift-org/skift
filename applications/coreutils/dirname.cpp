#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Stream.h>
#include <libsystem/io/Path.h>

static bool zero = false;

static const char *usages[] = {
    "NAME...",
    "[OPTION] NAME...",
    nullptr,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("zero", 'z', zero,
                         "end each output line with NUL, not newline",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_END};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Output each NAME with its last non-slash component and trailing slashes removed;",
    "if NAME contains no  /'s,  output  '.'  (meaning  the  current directory).");

char *get_dirname(char *raw_path)
{
    Path *path = path_create(raw_path);
    char *_dir;
    
    free(path_pop(path));
    
    if (path_element_count(path) == 0 && raw_path[0] != '/')
    {
        return nullptr;
    }

    _dir = path_as_string(path);
    
    path_destroy(path);

    return _dir;
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);
    
    if (argc == 1)
    {
        printf("dirname: missing operand\nTry 'dirname --help' for more information.\n");

        return PROCESS_FAILURE;
    }

    for (int i = 1; i < argc; i++)
    {
        char *dir_name = get_dirname(argv[i]);
        char terminator = zero ? '\0': '\n';

        //if nullptr is returned
        if (!dir_name)
        {
            printf(".%c", terminator);
        }
        else
        {
            printf("%s%c", dir_name, terminator);
        }
        
        free(dir_name);
    }

    return PROCESS_SUCCESS;
}

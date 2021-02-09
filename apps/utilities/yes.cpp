#include <libsystem/io/Stream.h>
#include <libutils/StringBuilder.h>

#include <stdio.h>

String concat(int argc, char **argv)
{
    int i;
    StringBuilder builder;

    for (i = 1; i < argc - 1; i++)
    {
        builder.append(argv[i]);
        builder.append(" ");
    }

    builder.append(argv[i]);

    return builder.finalize();
}

int main(int argc, char **argv)
{
    String output = "yes";

    if (argc > 1)
    {
        output = concat(argc, argv);
    }

    while (1)
    {
        printf("%s\n", output.cstring());

        if (handle_has_error(out_stream))
        {
            handle_printf_error(out_stream, "Couldn't write to stdout\n");
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}

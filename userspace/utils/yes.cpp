#include <libio/MemoryWriter.h>
#include <libio/Streams.h>

String concat(int argc, char **argv)
{
    IO::MemoryWriter memory;

    for (int i = 1; i < argc - 1; i++)
    {
        IO::write(memory, argv[i]);
        IO::write(memory, ' ');
    }

    return memory.string();
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
        auto result = IO::write(IO::out(), output);

        if (!result.success())
        {
            IO::outln("Couldn't write to stdout: {}", result.description());
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}

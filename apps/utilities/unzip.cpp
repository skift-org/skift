
#include <libsystem/io/File.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Stream.h>
#include <libutils/Path.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        stream_format(err_stream, "%s: Missing archive operand(s)\n", argv[0]);
        return PROCESS_FAILURE;
    }

    for(int i=1;i<argc;i++)
    {
      File file{argv[i]};

      ResultOr<Slice> result = file.read_all();

      if (result != SUCCESS)
      {
          stream_format(err_stream, "%s: Failed to read archive '%s'", argv[i], get_result_description(result.result()));
          return PROCESS_FAILURE;
      }

      
    }

    return PROCESS_SUCCESS;
}

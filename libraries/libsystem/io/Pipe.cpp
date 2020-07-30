#include <libsystem/core/Plugs.h>
#include <libsystem/io/Pipe.h>

Pipe *pipe_create()
{
    Pipe *pipe = __create(Pipe);

    int reader_handle = HANDLE_INVALID_ID;
    int writer_handle = HANDLE_INVALID_ID;

    __plug_create_pipe(&reader_handle, &writer_handle);

    pipe->in = stream_open_handle(writer_handle, OPEN_WRITE);
    pipe->out = stream_open_handle(reader_handle, OPEN_READ);

    return pipe;
}

void pipe_destroy(Pipe *pipe)
{
    stream_close(pipe->in);
    stream_close(pipe->out);
    free(pipe);
}

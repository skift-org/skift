#include <libgraphic/Bitmap.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Handle.h>
#include <libsystem/io/Socket.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

#include "compositor/Protocol.h"

int set_wallpaper(const char *path)
{
    Connection *compositor_connection = socket_connect("/Session/compositor.ipc");

    if (handle_has_error(compositor_connection))
    {
        handle_printf_error(compositor_connection, "Failed to connect to the compositor.");
        return -1;
    }

    auto wallaper_or_result = Bitmap::load_from(path);

    if (!wallaper_or_result.success())
    {
        stream_format(err_stream, "wallpaperctl: failed to load wallaper from %s: %s\n", path, result_to_string(wallaper_or_result.result()));
        return -1;
    }

    auto wallaper = wallaper_or_result.take_value();

    CompositorMessage message = (CompositorMessage){
        .type = COMPOSITOR_MESSAGE_SET_WALLPAPER,
        .set_wallaper = {
            .wallpaper = wallaper->handle(),
            .resolution = wallaper->size(),
        },
    };

    connection_send(compositor_connection, &message, sizeof(message));
    process_sleep(1000); // FIXME: Find a better way to wait for the server...

    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc == 2)
    {
        return set_wallpaper(argv[1]);
    }

    return 0;
}

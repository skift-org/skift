#include <abi/Paths.h>

#include <libutils/ArgParse.h>

#include <libio/Format.h>
#include <libio/Streams.h>
#include <libsystem/Result.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Socket.h>
#include <libsystem/io/Stream.h>

#include "compositor/Protocol.h"

static const IOCallDisplayModeArgs GFX_MODES[] = {
    {640, 360},
    {800, 600},
    {1024, 768},
    {1280, 720},
    {1280, 800},
    {1280, 1024},
    {1360, 768},
    {1366, 768},
    {1920, 1080},
    {3840, 2160},
};

Optional<IOCallDisplayModeArgs> gfxmode_by_name(String &name)
{
    for (size_t i = 0; i < __array_length(GFX_MODES); i++)
    {
        auto &gfx_mode = GFX_MODES[i];

        if (IO::format("{}x{}", gfx_mode.width, gfx_mode.height) == name)
        {
            return gfx_mode;
        }
    }

    return {};
}

int gfxmode_list()
{
    for (size_t i = 0; i < __array_length(GFX_MODES); i++)
    {
        auto &gfx_mode = GFX_MODES[i];

        IO::outln("- {}x{}", gfx_mode.width, gfx_mode.height);
    }

    return PROCESS_SUCCESS;
}

int gfxmode_get(Stream *framebuffer_device)
{
    IOCallDisplayModeArgs framebuffer_info;

    if (stream_call(framebuffer_device, IOCALL_DISPLAY_GET_MODE, &framebuffer_info) < 0)
    {
        handle_printf_error(framebuffer_device, "Ioctl to " FRAMEBUFFER_DEVICE_PATH " failed");
        return PROCESS_FAILURE;
    }

    IO::outln("Height: {}\nWidth: {}\n",
              framebuffer_info.width,
              framebuffer_info.height);

    return PROCESS_SUCCESS;
}

int gfxmode_set_compositor(IOCallDisplayModeArgs mode)
{
    Connection *compositor_connection = socket_connect("/Session/compositor.ipc");

    if (handle_has_error(compositor_connection))
    {
        handle_printf_error(compositor_connection, "Failed to connect to the compositor (Failling back on iocall)");
        return PROCESS_FAILURE;
    }

    CompositorMessage message = (CompositorMessage){
        .type = COMPOSITOR_MESSAGE_SET_RESOLUTION,
        .set_resolution = {
            mode.width,
            mode.height,
        },
    };

    connection_send(compositor_connection, &message, sizeof(message));

    return PROCESS_SUCCESS;
}

int gfxmode_set_iocall(Stream *device, IOCallDisplayModeArgs mode)
{
    if (stream_call(device, IOCALL_DISPLAY_SET_MODE, &mode) != SUCCESS)
    {
        handle_printf_error(device, "Ioctl to " FRAMEBUFFER_DEVICE_PATH " failed");
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}

int gfxmode_set(String &mode_name)
{
    auto mode = gfxmode_by_name(mode_name);

    if (!mode)
    {
        IO::errln("Error: unknow graphic mode: {}", mode_name);
        return PROCESS_FAILURE;
    }

    int result = gfxmode_set_compositor(*mode);

    if (result != 0)
    {
        __cleanup(stream_cleanup) Stream *device = stream_open(FRAMEBUFFER_DEVICE_PATH, OPEN_READ);
        result = gfxmode_set_iocall(device, *mode);
    }

    if (result == 0)
    {
        IO::outln("Graphic mode set to: {}", mode_name);
        return PROCESS_SUCCESS;
    }
    else
    {
        return PROCESS_FAILURE;
    }
}

int main(int argc, const char *argv[])
{

    ArgParse args{};

    args.show_help_if_no_option_given();
    args.should_abort_on_failure();

    args.usage("");
    args.usage("OPTION...");

    args.prologue("Get or set graphics modes.");

    args.option('l', "list", "List all available graphics modes.", [](auto &) {
        return gfxmode_list();
    });

    args.option('g', "get", "Get the current graphic mode.", [](auto &) {
        return gfxmode_list();
    });

    args.option_string('s', "set", "Set graphic mode.", [&](String &mode) {
        return gfxmode_set(mode);
    });

    args.epiloge("Options can be combined.");

    return args.eval(argc, argv);
}

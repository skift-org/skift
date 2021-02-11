#include <abi/Paths.h>

#include <libsystem/Result.h>
#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Socket.h>
#include <libsystem/io_new/Streams.h>

#include "compositor/Protocol.h"

static bool option_list = false;
static bool option_get = false;
static char *option_set = nullptr;

/* --- Command line application initialization -------------------------------*/

static const char *usages[] = {
    "",
    "OPTION...",
    nullptr,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("list", 'l', option_list, "List all available graphics modes.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("get", 'g', option_get, "Get the current graphic mode.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_STRING("set", 's', option_set, "Set graphic mode.", COMMANDLINE_NO_CALLBACK),

    COMMANDLINE_OPT_END,
};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Get or set graphics modes.",
    "Options can be combined.");

/* --- gfxmode logic -------------------------------------------------------- */

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

Optional<IOCallDisplayModeArgs> gfxmode_by_name(String name)
{
    for (size_t i = 0; i < __array_length(GFX_MODES); i++)
    {
        auto &gfx_mode = GFX_MODES[i];

        if (String::format("{}x{}", gfx_mode.width, gfx_mode.height) == name)
        {
            return gfx_mode;
        }
    }

    return {};
}

int gfxmode_get(Stream *framebuffer_device)
{
    IOCallDisplayModeArgs framebuffer_info;

    if (stream_call(framebuffer_device, IOCALL_DISPLAY_GET_MODE, &framebuffer_info) < 0)
    {
        handle_printf_error(framebuffer_device, "Ioctl to " FRAMEBUFFER_DEVICE_PATH " failed");
        return PROCESS_FAILURE;
    }

    System::outln("Height: {}\nWidth: {}\n",
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

int gfxmode_set(Stream *device, String mode_name)
{
    auto mode = gfxmode_by_name(mode_name);

    if (!mode)
    {
        System::errln("Error: unknow graphic mode: {}", mode_name);
        return PROCESS_FAILURE;
    }

    int result = gfxmode_set_compositor(*mode);

    if (result != 0)
    {
        result = gfxmode_set_iocall(device, *mode);
    }

    if (result == 0)
    {
        System::outln("Graphic mode set to: {}", mode_name);
        return PROCESS_SUCCESS;
    }
    else
    {
        return PROCESS_FAILURE;
    }
}

int gfxmode_list()
{
    for (size_t i = 0; i < __array_length(GFX_MODES); i++)
    {
        auto &gfx_mode = GFX_MODES[i];

        System::outln("- {}x{}", gfx_mode.width, gfx_mode.height);
    }

    return PROCESS_SUCCESS;
}

/* --- Entry point ---------------------------------------------------------- */

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    __cleanup(stream_cleanup) Stream *framebuffer_device = stream_open(FRAMEBUFFER_DEVICE_PATH, OPEN_READ);

    if (handle_has_error(HANDLE(framebuffer_device)))
    {
        handle_printf_error(framebuffer_device, "displayctl: Failed to open " FRAMEBUFFER_DEVICE_PATH);
        return PROCESS_FAILURE;
    }

    if (option_get)
    {
        return gfxmode_get(framebuffer_device);
    }
    else if (option_list)
    {
        return gfxmode_list();
    }
    else if (option_set != nullptr)
    {
        return gfxmode_set(framebuffer_device, option_set);
    }
    else
    {
        return gfxmode_get(framebuffer_device);
    }
}

#include <abi/Paths.h>

#include <libsystem/Result.h>
#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Socket.h>
#include <libsystem/io/Stream.h>

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

struct SupportedMode
{
    const char *name;
    IOCallDisplayModeArgs info;
};

SupportedMode gfxmodes[] = {
    {"640x360", {640, 360}},
    {"800x600", {800, 600}},
    {"1024x768", {1024, 768}},
    {"1280x720", {1280, 720}},
    {"1280x800", {1280, 800}},
    {"1280x1024", {1280, 1024}},
    {"1360x768", {1360, 768}},
    {"1366x768", {1366, 768}},
    {"1920x1080", {1920, 1080}},
    {"3840x2160", {3840, 2160}},
    {nullptr, {0, 0}},
};

IOCallDisplayModeArgs *gfxmode_by_name(const char *name)
{
    for (int i = 0; gfxmodes[i].name; i++)
    {
        if (strcmp(gfxmodes[i].name, name) == 0)
        {
            return &gfxmodes[i].info;
        }
    }

    return nullptr;
}

int gfxmode_get(Stream *framebuffer_device)
{
    IOCallDisplayModeArgs framebuffer_info;

    if (stream_call(framebuffer_device, IOCALL_DISPLAY_GET_MODE, &framebuffer_info) < 0)
    {
        handle_printf_error(framebuffer_device, "Ioctl to " FRAMEBUFFER_DEVICE_PATH " failed");
        return -1;
    }

    printf("Height: %d\nWidth: %d\n",
           framebuffer_info.width,
           framebuffer_info.height);

    return 0;
}

int gfxmode_set_compositor(IOCallDisplayModeArgs *mode)
{
    Connection *compositor_connection = socket_connect("/Session/compositor.ipc");

    if (handle_has_error(compositor_connection))
    {
        handle_printf_error(compositor_connection, "Failed to connect to the compositor (Failling back on iocall)");
        return -1;
    }

    CompositorMessage message = (CompositorMessage){
        .type = COMPOSITOR_MESSAGE_SET_RESOLUTION,
        .set_resolution = {
            mode->width,
            mode->height,
        },
    };

    connection_send(compositor_connection, &message, sizeof(message));

    return 0;
}

int gfxmode_set_iocall(Stream *device, IOCallDisplayModeArgs *mode)
{
    if (stream_call(device, IOCALL_DISPLAY_SET_MODE, mode) != SUCCESS)
    {
        handle_printf_error(device, "Ioctl to " FRAMEBUFFER_DEVICE_PATH " failed");
        return -1;
    }

    return 0;
}

int gfxmode_set(Stream *device, const char *mode_name)
{
    IOCallDisplayModeArgs *mode = gfxmode_by_name(mode_name);

    if (!mode)
    {
        printf("Error: unknow graphic mode: %s\n", mode_name);
        return -1;
    }

    int result = gfxmode_set_compositor(mode);

    if (result != 0)
    {
        result = gfxmode_set_iocall(device, mode);
    }

    if (result == 0)
    {
        printf("Graphic mode set to: %s\n", mode_name);
        return 0;
    }
    else
    {
        return -1;
    }
}

int gfxmode_list(Stream *framebuffer_device)
{
    // FIXME: check if the framebuffer device support the followings graphics modes.

    __unused(framebuffer_device);

    for (int i = 0; gfxmodes[i].name; i++)
    {
        printf("%s\n", gfxmodes[i].name);
    }

    return 0;
}

/* --- Entry point ---------------------------------------------------------- */

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    __cleanup(stream_cleanup) Stream *framebuffer_device = stream_open(FRAMEBUFFER_DEVICE_PATH, OPEN_READ);

    if (handle_has_error(HANDLE(framebuffer_device)))
    {
        handle_printf_error(framebuffer_device, "displayctl: Failed to open " FRAMEBUFFER_DEVICE_PATH);
        return -1;
    }

    if (option_get)
    {
        return gfxmode_get(framebuffer_device);
    }
    else if (option_list)
    {
        return gfxmode_list(framebuffer_device);
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

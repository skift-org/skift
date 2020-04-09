/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>
#include <libsystem/cmdline.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>

static bool option_list = false;
static bool option_get = false;
static char *option_set = NULL;

/* --- Command line application initialization -------------------------------*/

static const char *usages[] = {
    "",
    "OPTION...",
    NULL,
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

typedef struct
{
    const char *name;
    IOCallDisplayModeArgs info;
} SupportedMode;

SupportedMode gfxmodes[] = {
    {"640x360", {640, 360}},
    {"800x600", {800, 600}},
    {"1024x768", {1024, 768}},
    {"1280x768", {1280, 768}},
    {NULL, {0, 0}},
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

    return NULL;
}

int gfxmode_get(Stream *framebuffer_device)
{
    IOCallDisplayModeArgs framebuffer_info;

    if (stream_call(framebuffer_device, IOCALL_DISPLAY_GET_MODE, &framebuffer_info) < 0)
    {
        error_print("Ioctl to /dev/framebuffer failled");
        return -1;
    }

    printf("Height: %d\nWidth: %d\n",
           framebuffer_info.width,
           framebuffer_info.height);

    return 0;
}

int gfxmode_set(Stream *framebuffer_device, const char *mode_name)
{
    IOCallDisplayModeArgs *framebuffer_info = gfxmode_by_name(mode_name);

    if (framebuffer_info != NULL)
    {
        if (stream_call(framebuffer_device, IOCALL_DISPLAY_SET_MODE, framebuffer_info) < 0)
        {
            error_print("Ioctl to /dev/framebuffer failled");
            return -1;
        }

        printf("Graphic mode set to: %s\n", mode_name);

        return 0;
    }
    else
    {
        printf("Error: unknow graphic mode: %s\n", mode_name);
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

    Stream *framebuffer_device = stream_open("/dev/framebuffer", OPEN_READ);

    if (handle_has_error(HANDLE(framebuffer_device)))
    {
        handle_printf_error(framebuffer_device, "displayctl: Failled to open /dev/framebuffer");
        stream_close(framebuffer_device);

        return -1;
    }

    int result = -1;

    if (option_get)
    {
        result = gfxmode_get(framebuffer_device);
    }
    else if (option_list)
    {
        result = gfxmode_list(framebuffer_device);
    }
    else if (option_set != NULL)
    {
        result = gfxmode_set(framebuffer_device, option_set);
    }
    else
    {
        result = gfxmode_get(framebuffer_device);
    }

    stream_close(framebuffer_device);

    return result;
}

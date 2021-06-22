#include <abi/Paths.h>

#include <libio/Format.h>
#include <libio/Socket.h>
#include <libio/Streams.h>
#include <libshell/ArgParse.h>
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
    for (size_t i = 0; i < ARRAY_LENGTH(GFX_MODES); i++)
    {
        auto &gfx_mode = GFX_MODES[i];

        if (IO::format("{}x{}", gfx_mode.width, gfx_mode.height) == name)
        {
            return gfx_mode;
        }
    }

    return NONE;
}

Shell::ArgParseResult gfxmode_list()
{
    for (size_t i = 0; i < ARRAY_LENGTH(GFX_MODES); i++)
    {
        auto &gfx_mode = GFX_MODES[i];

        IO::outln("- {}x{}", gfx_mode.width, gfx_mode.height);
    }

    return Shell::ArgParseResult::SHOULD_FINISH;
}

Shell::ArgParseResult gfxmode_get()
{
    IOCallDisplayModeArgs framebuffer_info;

    Stream *device = stream_open(FRAMEBUFFER_DEVICE_PATH, HJ_OPEN_READ);
    if (stream_call(device, IOCALL_DISPLAY_GET_MODE, &framebuffer_info) != SUCCESS)
    {
        IO::errln("Ioctl to " FRAMEBUFFER_DEVICE_PATH " failed");
        return Shell::ArgParseResult::FAILURE;
    }

    IO::outln("Height: {}\nWidth: {}\n",
              framebuffer_info.width,
              framebuffer_info.height);

    return Shell::ArgParseResult::SHOULD_FINISH;
}

HjResult gfxmode_set_compositor(IOCallDisplayModeArgs mode)
{
    auto connection = TRY(IO::Socket::connect("/Session/compositor.ipc"));

    CompositorMessage message{
        .type = COMPOSITOR_MESSAGE_SET_RESOLUTION,
        .set_resolution = {
            mode.width,
            mode.height,
        },
    };

    TRY(connection.write(&message, sizeof(message)));

    process_sleep(1000);

    CompositorMessage goodbye_message{
        .type = COMPOSITOR_MESSAGE_GOODBYE,
        .greetings = {},
    };

    TRY(connection.write(&goodbye_message, sizeof(goodbye_message)));

    return HjResult::SUCCESS;
}

HjResult gfxmode_set_iocall(Stream *device, IOCallDisplayModeArgs mode)
{
    if (stream_call(device, IOCALL_DISPLAY_SET_MODE, &mode) != SUCCESS)
    {
        IO::errln("Ioctl to " FRAMEBUFFER_DEVICE_PATH " failed");
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }

    return HjResult::SUCCESS;
}

Shell::ArgParseResult gfxmode_set(String &mode_name)
{
    auto mode = gfxmode_by_name(mode_name);

    if (!mode.present())
    {
        IO::errln("Error: unknown graphic mode: {}", mode_name);
        return Shell::ArgParseResult::FAILURE;
    }

    HjResult result = gfxmode_set_compositor(mode.unwrap());

    if (result != HjResult::SUCCESS)
    {
        CLEANUP(stream_cleanup)
        Stream *device = stream_open(FRAMEBUFFER_DEVICE_PATH, HJ_OPEN_READ);
        result = gfxmode_set_iocall(device, mode.unwrap());
    }

    if (result == HjResult::SUCCESS)
    {
        IO::outln("Graphic mode set to: {}", mode_name);
        return Shell::ArgParseResult::SHOULD_FINISH;
    }
    else
    {
        return Shell::ArgParseResult::FAILURE;
    }
}

int main(int argc, const char *argv[])
{
    Shell::ArgParse args{};

    args.show_help_if_no_option_given();
    args.should_abort_on_failure();

    args.usage("");
    args.usage("OPTION...");

    args.prologue("Get or set graphics modes.");

    args.option('l', "list", "List all available graphics modes.", [](auto &) {
        return gfxmode_list();
    });

    args.option('g', "get", "Get the current graphic mode.", [](auto &) {
        return gfxmode_get();
    });

    args.option_string('s', "set", "Set graphic mode.", [&](String &mode) {
        return gfxmode_set(mode);
    });

    args.epiloge("Options can be combined.");

    return args.eval(argc, argv) == Shell::ArgParseResult::FAILURE
               ? PROCESS_FAILURE
               : PROCESS_SUCCESS;
}

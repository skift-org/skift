#include <abi/Paths.h>

#include <libutils/ArgParse.h>

#include <libio/Format.h>
#include <libio/Socket.h>
#include <libio/Streams.h>

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
    for (size_t i = 0; i < AERAY_LENGTH(GFX_MODES); i++)
    {
        auto &gfx_mode = GFX_MODES[i];

        if (IO::format("{}x{}", gfx_mode.width, gfx_mode.height) == name)
        {
            return gfx_mode;
        }
    }

    return {};
}

ArgParseResult gfxmode_list()
{
    for (size_t i = 0; i < AERAY_LENGTH(GFX_MODES); i++)
    {
        auto &gfx_mode = GFX_MODES[i];

        IO::outln("- {}x{}", gfx_mode.width, gfx_mode.height);
    }

    return ArgParseResult::SHOULD_FINISH;
}

ArgParseResult gfxmode_get(Stream *framebuffer_device)
{
    IOCallDisplayModeArgs framebuffer_info;

    if (stream_call(framebuffer_device, IOCALL_DISPLAY_GET_MODE, &framebuffer_info) < 0)
    {
        handle_printf_error(framebuffer_device, "Ioctl to " FRAMEBUFFER_DEVICE_PATH " failed");
        return ArgParseResult::FAILURE;
    }

    IO::outln("Height: {}\nWidth: {}\n",
              framebuffer_info.width,
              framebuffer_info.height);

    return ArgParseResult::SHOULD_FINISH;
}

Result gfxmode_set_compositor(IOCallDisplayModeArgs mode)
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

    return Result::SUCCESS;
}

Result gfxmode_set_iocall(Stream *device, IOCallDisplayModeArgs mode)
{
    if (stream_call(device, IOCALL_DISPLAY_SET_MODE, &mode) != SUCCESS)
    {
        handle_printf_error(device, "Ioctl to " FRAMEBUFFER_DEVICE_PATH " failed");
        return Result::ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }

    return Result::SUCCESS;
}

ArgParseResult gfxmode_set(String &mode_name)
{
    auto mode = gfxmode_by_name(mode_name);

    if (!mode.present())
    {
        IO::errln("Error: unknow graphic mode: {}", mode_name);
        return ArgParseResult::FAILURE;
    }

    Result result = gfxmode_set_compositor(mode.unwrap());

    if (result != Result::SUCCESS)
    {
        CLEANUP(stream_cleanup)
        Stream *device = stream_open(FRAMEBUFFER_DEVICE_PATH, OPEN_READ);
        result = gfxmode_set_iocall(device, mode.unwrap());
    }

    if (result == Result::SUCCESS)
    {
        IO::outln("Graphic mode set to: {}", mode_name);
        return ArgParseResult::SHOULD_FINISH;
    }
    else
    {
        return ArgParseResult::FAILURE;
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

    return args.eval(argc, argv) == ArgParseResult::FAILURE ? PROCESS_FAILURE : PROCESS_SUCCESS;
}

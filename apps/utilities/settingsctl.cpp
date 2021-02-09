#include <libutils/ArgParse.h>

#include <libsystem/eventloop/EventLoop.h>

#include <libsettings/Settings.h>
#include <libsettings/Watcher.h>

int main(int argc, const char **argv)
{
    ArgParse args;

    args.should_abort_on_failure();
    args.show_help_if_no_option_given();

    args.prologue("Read, write, or, watch system settings.");

    args.usage("-r SETTINGS...");
    args.usage("-w SETTING VALUE");
    args.usage("--watch SETTINGS...");

    args.option_string('r', "read", "Read settings from the settings storage.", [&](auto &key_name) {
        auto maybe_settings = Settings::read(Settings::Path::parse(key_name));

        if (!maybe_settings)
        {
            stream_format(err_stream, "%s: No such setting.\n", argv[0]);
            process_exit(PROCESS_FAILURE);
        }

        Prettifier pretty;
        json::prettify(pretty, *maybe_settings);

        stream_format(out_stream, "%s", pretty.finalize().cstring());
    });

    args.option('w', "write", "Write a setting to the settings storage.", [&](ArgParseContext &context) {
        auto maybe_key_name = context.pop_operand();

        if (!maybe_key_name)
        {
            args.usage();
        }

        auto maybe_value = context.pop_operand();

        if (!maybe_value)
        {
            args.usage();
        }

        Settings::write(Settings::Path::parse(*maybe_key_name), json::parse(*maybe_value));
    });

    OwnPtr<Settings::Watcher> watcher = nullptr;

    args.option(0, "watch", "Watch settings for changes.", [&](ArgParseContext &context) {
        auto maybe_key_name = context.pop_operand();

        if (!maybe_key_name)
        {
            args.usage();
        }

        EventLoop::initialize();

        watcher = own<Settings::Watcher>(Settings::Path::parse(*maybe_key_name), [&](auto &value) {
            Prettifier pretty;
            json::prettify(pretty, value);

            stream_format(out_stream, "%s\n", pretty.finalize().cstring());
        });

        process_exit(EventLoop::run());
    });

    args.epiloge("Option cannot be combined.");

    return args.eval(argc, argv);
}

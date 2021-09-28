#include <libasync/Loop.h>
#include <libsettings/Service.h>
#include <libsettings/Watcher.h>
#include <libshell/ArgParse.h>

int main(int argc, const char **argv)
{
    Shell::ArgParse args;

    args.should_abort_on_failure();
    args.show_help_if_no_option_given();

    args.prologue("Read, write, or, watch system settings.");

    args.usage("-r SETTINGS...");
    args.usage("-w SETTING VALUE");
    args.usage("--watch SETTINGS...");

    args.option_string('r', "read", "Read settings from the settings storage.", [&](auto &key_name) {
        auto maybe_settings = Settings::Service::the()->read(Settings::Path::parse(key_name));

        if (!maybe_settings.present())
        {
            IO::errln("{}: No such setting.", argv[0]);
            return Shell::ArgParseResult::FAILURE;
        }

        IO::outln("{}", Json::stringify(maybe_settings.unwrap()));
        return Shell::ArgParseResult::SHOULD_FINISH;
    });

    args.option('w', "write", "Write a setting to the settings storage.", [&](Shell::ArgParseContext &context) {
        auto maybe_key_name = context.pop_operand();

        if (!maybe_key_name.present())
        {
            args.usage();
        }

        auto maybe_value = context.pop_operand();

        if (!maybe_value.present())
        {
            args.usage();
        }

        Settings::Service::the()->write(Settings::Path::parse(maybe_key_name.unwrap()), Json::parse(maybe_value.unwrap()));

        return Shell::ArgParseResult::SHOULD_FINISH;
    });

    OwnPtr<Settings::Watcher> watcher = nullptr;

    args.option(0, "watch", "Watch settings for changes.", [&](Shell::ArgParseContext &context) {
        auto maybe_key_name = context.pop_operand();

        if (!maybe_key_name.present())
        {
            args.usage();
        }

        watcher = own<Settings::Watcher>(Settings::Path::parse(maybe_key_name.unwrap()), [&](auto &value) {
            IO::outln("{}", Json::stringify(value));
        });

        return Async::Loop::the()->run() == 0
                   ? Shell::ArgParseResult::SHOULD_FINISH
                   : Shell::ArgParseResult::FAILURE;
    });

    args.epiloge("Option cannot be combined.");

    return args.eval(argc, argv) == Shell::ArgParseResult::FAILURE ? PROCESS_FAILURE : PROCESS_SUCCESS;
}

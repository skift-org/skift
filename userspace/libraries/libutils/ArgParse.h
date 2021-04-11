#pragma once

#include <libio/MemoryReader.h>
#include <libio/NumberScanner.h>
#include <libio/Scanner.h>
#include <libio/Streams.h>

#include <libsystem/process/Process.h>

#include <libutils/Callback.h>
#include <libutils/Optional.h>
#include <libutils/String.h>
#include <libutils/Traits.h>
#include <libutils/Vector.h>

class ArgParseContext
{
private:
    Vector<String> _arguments;

public:
    ArgParseContext(int argc, char const *argv[])
    {
        for (int i = 0; i < argc; i++)
        {
            _arguments.push_back(argv[i]);
        }
    }

    bool any()
    {
        return _arguments.any();
    }

    const String &current()
    {
        return _arguments.peek();
    }

    Vector<String> pop_operands()
    {
        Vector<String> operands;

        while (any() && current()[0] != '-')
        {
            operands.push_back(pop());
        }

        return operands;
    }

    Optional<String> pop_operand()
    {
        if (current()[0] != '-')
        {
            return pop();
        }
        else
        {
            return {};
        }
    }

    String pop()
    {
        return _arguments.pop();
    }
};

using ArgParseOptionCallback = Callback<int(ArgParseContext &)>;

struct ArgParseOption
{
    char shortname;
    String longname{};
    String description{};
    ArgParseOptionCallback callback;
};

class ArgParse
{
private:
    String _name;
    Vector<String> _usages;
    Vector<ArgParseOption> _option;
    String _prologue;
    String _epiloge;
    Vector<String> _argv;

    bool _should_abort_on_failure = false;
    bool _show_help_if_no_operand_given = false;
    bool _show_help_if_no_option_given = false;

public:
    static constexpr char NO_SHORT_NAME = '\0';

    void prologue(String prologue) { _prologue = prologue; }

    void usage(String usage) { _usages.push_back(usage); }

    void epiloge(String epiloge) { _epiloge = epiloge; }

    auto argc() { return _argv.count(); }

    auto &argv() { return _argv; }

    void should_abort_on_failure() { _should_abort_on_failure = true; }

    void show_help_if_no_operand_given() { _show_help_if_no_operand_given = true; }

    void show_help_if_no_option_given() { _show_help_if_no_option_given = true; }

    ArgParse()
    {
        _option.push_back(
            {
                'h',
                "help",
                "Show this help message and exit.",
                [&](auto &) {
                    return help();
                },
            });
    }

    void option(char shortname, String longname, String description, ArgParseOptionCallback callback)
    {
        _option.push_back({
            shortname,
            longname,
            description,
            callback,
        });
    }

    void option_string(char shortname, String longname, String description, Callback<int(String &)> callback)
    {
        _option.push_back({
            shortname,
            longname,
            description,

            [this, callback](ArgParseContext &ctx) {
                auto maybe_strings = ctx.pop_operand();

                if (!maybe_strings.present())
                {
                    return usage();
                }

                return callback(maybe_strings.unwrap());
            },
        });
    }

    void option_int(char shortname, String longname, String description, Callback<int(int)> callback)
    {
        _option.push_back({
            shortname,
            longname,
            description,

            [this, callback](ArgParseContext &ctx) {
                auto maybe_string = ctx.pop_operand();

                if (!maybe_string.present())
                {
                    return usage();
                }

                IO::MemoryReader memory{maybe_string.unwrap()};
                IO::Scanner scan{memory};

                auto maybe_value = IO::NumberScanner::decimal().scan_int(scan);

                if (!maybe_value.present())
                {
                    return usage();
                }

                return callback(maybe_value.unwrap());
            },
        });
    }

    void option_bool(char shortname, String longname, String description, Callback<int(bool)> callback)
    {
        option(shortname, longname, description, [this, callback](ArgParseContext &ctx) {
            bool value = true;

            if (ctx.any())
            {
                if (ctx.current() == "true" ||
                    ctx.current() == "yes")
                {
                    value = true;
                    ctx.pop();
                }

                if (ctx.current() == "false" ||
                    ctx.current() == "no")
                {
                    value = false;
                    ctx.pop();
                }
            }

            return callback(value);
        });
    }

    void option(bool &value, char shortname, String longname, String description)
    {
        option_bool(shortname, longname, description, [&](bool v) {
            value = v;
            return PROCESS_SUCCESS;
        });
    }

    void option(String &value, char shortname, String longname, String description)
    {
        option(shortname, longname, description, [&](auto &ctx) {
            if (ctx.any())
            {
                value = ctx.pop();
                return PROCESS_SUCCESS;
            }
            else
            {
                return usage();
            }
        });
    }

    int help()
    {
        if (!_prologue.null_or_empty())
        {
            IO::outln("{}\n", _prologue);
        }

        if (_usages.any())
        {
            IO::outln("\e[1mUsages:\e[0m");

            for (size_t i = 0; i < _usages.count(); i++)
            {
                IO::outln("    {} {}", _name, _usages[i]);
            }

            IO::outln("");
        }

        if (_option.any())
        {
            auto compute_padding = [&](auto &options) {
                size_t max = 0;

                for (size_t i = 0; i < options.count(); i++)
                {
                    if (!options[i].longname.null_or_empty())
                    {
                        max = MAX(max, options[i].longname.length());
                    }
                }

                return max;
            };

            auto padding = compute_padding(_option);

            IO::outln("\e[1mOptions:\e[0m");

            for (size_t i = 0; i < _option.count(); i++)
            {
                auto &opt = _option[i];

                IO::write(IO::out(), "    ");

                if (opt.shortname != '\0')
                {
                    IO::format(IO::out(), "-{c}", opt.shortname);
                }
                else
                {
                    IO::write(IO::out(), "  ");
                }

                if (opt.shortname && !opt.longname.null_or_empty())
                {
                    IO::write(IO::out(), ", ");
                }
                else
                {
                    IO::write(IO::out(), "  ");
                }

                if (!opt.longname.null_or_empty())
                {
                    IO::format(IO::out(), "--{}", opt.longname);

                    if (padding > opt.longname.length())
                    {
                        size_t computed_padding = padding - opt.longname.length();

                        for (size_t i = 0; i < computed_padding; i++)
                        {
                            IO::write(IO::out(), " ");
                        }
                    }
                }
                else
                {
                    for (size_t i = 0; i < padding; i++)
                    {
                        IO::write(IO::out(), " ");
                    }
                }

                if (!opt.description.null_or_empty())
                {
                    IO::format(IO::out(), " {}", opt.description);
                }

                IO::write(IO::out(), "\n");
            }

            IO::write(IO::out(), "\n");
        }

        if (!_epiloge.null_or_empty())
        {
            IO::outln("{}", _epiloge);
        }

        return PROCESS_SUCCESS;
    }

    int fail()
    {
        IO::errln("Try '{} --help' for more information.\n", _name);
        return PROCESS_FAILURE;
    }

    int usage()
    {
        IO::errln("{}: missing operand.", _name.cstring());
        return fail();
    }

    int invalid_option(String option)
    {
        IO::errln("{}: invalide option '{}'!", _name.cstring(), option.cstring());
        return fail();
    }

    [[nodiscard]] int eval(int argc, char const *argv[])
    {
        ArgParseContext context{argc, argv};

        _name = context.pop();

        auto has_run_option = false;

        while (context.any())
        {
            auto current = context.pop();

            IO::MemoryReader memory{current};
            IO::Scanner scan{memory};

            if (scan.skip_word("--"))
            {
                String argument{current.cstring() + 2, current.length() - 2};

                bool found_valid_option = false;

                for (size_t i = 0; i < _option.count(); i++)
                {
                    if (_option[i].longname == argument)
                    {
                        int result = _option[i].callback(context);

                        if (result != PROCESS_SUCCESS)
                        {
                            if (_should_abort_on_failure)
                            {
                                process_exit(PROCESS_FAILURE);
                            }

                            return result;
                        }

                        has_run_option = true;
                        found_valid_option = true;
                    }
                }

                if (!found_valid_option)
                {
                    return invalid_option(current);
                }
            }
            else if (scan.skip('-'))
            {
                while (scan.do_continue())
                {
                    bool found_valid_option = false;

                    for (size_t i = 0; i < _option.count(); i++)
                    {
                        if (_option[i].shortname == scan.current())
                        {
                            int result = _option[i].callback(context);

                            if (result != PROCESS_SUCCESS)
                            {
                                if (_should_abort_on_failure)
                                {
                                    process_exit(PROCESS_FAILURE);
                                }

                                return result;
                            }

                            has_run_option = true;
                            found_valid_option = true;
                        }
                    }

                    if (!found_valid_option)
                    {
                        return invalid_option(scan.current());
                    }

                    scan.forward();
                }
            }
            else
            {
                _argv.push_back(current);
            }
        }

        if (!this->argv().any() && _show_help_if_no_operand_given)
        {
            return usage();
        }

        if (!has_run_option && _show_help_if_no_option_given)
        {
            return usage();
        }

        return PROCESS_SUCCESS;
    }
};

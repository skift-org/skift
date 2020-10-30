#pragma once

#include <libsystem/utils/Lexer.h>
#include <libutils/Callback.h>
#include <libutils/String.h>
#include <libutils/Traits.h>
#include <libutils/Vector.h>

class EvalContext
{
private:
    Vector<String> _arguments;

public:
    EvalContext(int argc, char const *argv[])
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

    String pop()
    {
        return _arguments.pop();
    }
};

class Option : public RefCounted<Option>
{
private:
    char _shortname;
    String _longname{};
    String _description{};

public:
    char shortname() { return _shortname; }

    const String &longname() { return _longname; }

    void longname(String longname) { _longname = longname; }

    const String &description() { return _description; }

    void description(String description) { _description = description; }

    Option(char shortname) : _shortname(shortname) {}

    virtual ~Option() {}

    virtual void eval(EvalContext){};
};

template <typename T>
concept ArgOption = IsBaseOf<Option, T>::value;

template <typename TValue>
class ValueOption : public Option
{
private:
    TValue _value{};

public:
    TValue value() { return _value; }

    void value(TValue value) { _value = value; }

    ValueOption(char shortname) : Option(shortname) {}
};

class OptionBool : public ValueOption<bool>
{
public:
    OptionBool(char shortname) : ValueOption(shortname) {}

    virtual void eval(EvalContext)
    {
        value(true);
    }
};

class OptionString : public ValueOption<String>
{
public:
    OptionString(char shortname) : ValueOption(shortname) {}

    virtual void eval(EvalContext context)
    {
        if (context.any())
        {
            value(context.pop());
        }
    }
};

class OptionInt : public ValueOption<int>
{
};

class OptionCallback : public Option
{
private:
    Callback<void()> _callback;

public:
    OptionCallback(char shortname, Callback<void()> callback) : Option(shortname), _callback(callback)
    {
    }

    virtual void eval(EvalContext)
    {
        if (_callback)
        {
            _callback();
        }
    }
};

class ArgParse
{
private:
    String _name;
    Vector<String> _usages;
    Vector<RefPtr<Option>> _option;
    String _prologue;
    String _epiloge;
    Vector<String> _argv;

    bool _show_help_if_no_operand_given = false;

public:
    ArgParse()
    {
        auto help_option = option<OptionCallback>('h', [&]() { help(); });
        help_option->longname("help");
        help_option->description("Show this help message and exit.");
    }

    auto argc() { return _argv.count(); }

    auto &argv() { return _argv; }

    void show_help_if_no_operand_given()
    {
        _show_help_if_no_operand_given = true;
    }

    template <ArgOption TOption, typename... TArgs>
    RefPtr<TOption> option(TArgs... args)
    {
        auto option = make<TOption>(forward<TArgs>(args)...);
        _option.push_back(option);
        return option;
    }

    void prologue(String prologue)
    {
        _prologue = prologue;
    }

    void usage(String usage)
    {
        _usages.push_back(usage);
    }

    void epiloge(String epiloge)
    {
        _epiloge = epiloge;
    }

    void help()
    {
        if (!_prologue.null_or_empty())
        {
            printf("%s\n", _prologue.cstring());
            printf("\n");
        }

        if (_usages.any())
        {
            printf("\e[1mUsages:\e[0m\n");

            for (size_t i = 0; i < _usages.count(); i++)
            {
                printf("    %s %s\n", _name.cstring(), _usages[i].cstring());
            }

            printf("\n");
        }

        if (_option.any())
        {
            auto compute_padding = [&](auto &options) {
                size_t max = 0;

                for (size_t i = 0; i < options.count(); i++)
                {
                    if (!options[i]->longname().null_or_empty())
                    {
                        max = MAX(max, options[i]->longname().length());
                    }
                }

                return max;
            };

            auto padding = compute_padding(_option);

            printf("\e[1mOptions:\e[0m\n");

            for (size_t i = 0; i < _option.count(); i++)
            {
                auto &opt = _option[i];

                printf("    ");

                if (opt->shortname() != '\0')
                {
                    printf("-%c", opt->shortname());
                }
                else
                {
                    printf("   ");
                }

                if (opt->shortname() && !opt->longname().null_or_empty())
                {
                    printf(", ");
                }
                else
                {
                    printf("  ");
                }

                if (!opt->longname().null_or_empty())
                {
                    printf("--%s", opt->longname().cstring());

                    if (padding > opt->longname().length())
                    {
                        size_t computed_padding = padding - opt->longname().length();

                        for (size_t i = 0; i < computed_padding; i++)
                        {
                            printf(" ");
                        }
                    }
                }
                else
                {
                    for (size_t i = 0; i < padding; i++)
                    {
                        printf(" ");
                    }
                }

                if (!opt->description().null_or_empty())
                {
                    printf(" ");
                    printf("%s", opt->description().cstring());
                }

                printf("\n");
            }

            printf("\n");
        }

        if (!_epiloge.null_or_empty())
        {
            printf("%s\n", _epiloge.cstring());
        }
    }

    void fail()
    {
        stream_format(err_stream, "Try '%s --help' for more information.\n", _name.cstring());
    }

    void usage()
    {
        stream_format(err_stream, "%s: missing operand.\n", _name.cstring());
        fail();
    }

    void invalid_option(String option)
    {
        stream_format(err_stream, "%s: invalide option '%s'!\n", _name.cstring(), option.cstring());
        fail();
    }

    int eval(int argc, char const *argv[])
    {
        EvalContext context{argc, argv};

        _name = context.pop();

        while (context.any())
        {
            auto current = context.pop();
            Lexer lexer{current.cstring(), current.length()};

            if (lexer.skip_word("--"))
            {
                auto argument = current.view(2, current.length() - 2);

                bool found_valid_option = false;

                for (size_t i = 0; i < _option.count(); i++)
                {
                    if (_option[i]->longname() == argument)
                    {
                        _option[i]->eval(context);

                        found_valid_option = true;
                    }
                }

                if (!found_valid_option)
                {
                    invalid_option(current);
                    return PROCESS_FAILURE;
                }
            }
            else if (lexer.skip('-'))
            {
                while (lexer.do_continue())
                {
                    bool found_valid_option = false;

                    for (size_t i = 0; i < _option.count(); i++)
                    {
                        if (_option[i]->shortname() == lexer.current())
                        {
                            _option[i]->eval(context);

                            found_valid_option = true;
                        }
                    }

                    if (!found_valid_option)
                    {
                        invalid_option(lexer.current());
                        return PROCESS_FAILURE;
                    }

                    lexer.foreward();
                }
            }
            else
            {
                _argv.push_back(current);
            }
        }

        if (!this->argv().any() && _show_help_if_no_operand_given)
        {
            usage();
            return PROCESS_FAILURE;
        }

        return PROCESS_SUCCESS;
    }
};

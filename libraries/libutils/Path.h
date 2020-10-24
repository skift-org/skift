#pragma once

#include <abi/Filesystem.h>

#include <libsystem/utils/Lexer.h>
#include <libutils/String.h>
#include <libutils/StringBuilder.h>
#include <libutils/Vector.h>

struct Path
{
private:
    bool _absolute = false;
    Vector<String> _elements{};

public:
    static constexpr int PARENT_SHORTHAND = 1; // .... -> ../../..

    bool absolute() const { return _absolute; }

    bool relative() const { return !_absolute; }

    size_t length() const { return _elements.count(); }

    static Path parse(const String &string)
    {
        return parse(string.cstring(), string.length());
    }

    static Path parse(const char *path)
    {
        return parse(path, strlen(path));
    }

    static Path parse(const char *path, size_t size, int flags = 0)
    {
        Lexer lexer{path, size};
        StringBuilder builder{};

        bool absolute = false;

        if (lexer.skip(PATH_SEPARATOR))
        {
            absolute = true;
        }

        Vector<String> elements{};

        while (lexer.do_continue())
        {
            while (!lexer.skip(PATH_SEPARATOR) && lexer.do_continue())
            {
                if ((flags & PARENT_SHORTHAND) && lexer.skip_word(".."))
                {
                    elements.push("..");

                    while (lexer.skip('.'))
                    {
                        elements.push("..");
                    }
                }

                builder.append(lexer.current());
                lexer.foreward();
            }

            if (builder.length() > 0)
            {
                elements.push_back(builder.finalize());
            }
        }

        return {absolute, move(elements)};
    }

    static Path join(Path &left, String right)
    {
        return join(left, parse(right));
    }

    static Path join(Path &left, Path &&right)
    {
        return join(left, right);
    }

    static Path join(Path &left, Path &right)
    {
        Vector<String> combined_elements{};

        combined_elements.push_back_many(left._elements);
        combined_elements.push_back_many(right._elements);

        return {left.absolute(), move(combined_elements)};
    }

    Path(const Path &other)
        : _absolute{other.absolute()}, _elements{other._elements}
    {
    }

    Path(bool absolute, Vector<String> &&elements)
        : _absolute(absolute), _elements(elements)
    {
    }

    Path(Path &&other)
    {
        swap(_absolute, other._absolute);
        swap(_elements, other._elements);
    }

    Path &operator=(const Path &other)
    {
        if (this != &other)
        {
            _absolute = other.absolute();
            _elements = other._elements;
        }

        return *this;
    }

    Path &operator=(Path &&other)
    {
        if (this != &other)
        {
            swap(_absolute, other._absolute);
            swap(_elements, other._elements);
        }

        return *this;
    }

    String operator[](size_t index) const
    {
        return _elements[index];
    }

    bool operator==(const Path &other)
    {
        if (this == &other)
            return true;

        if (_absolute == other._absolute)
            return false;

        return _elements == other._elements;
    }

    Path normalized()
    {
        Vector<String> stack{};

        _elements.foreach ([&](auto &element) {
            if (element == ".." && stack.count() > 0)
            {
                stack.pop_back();
            }
            else if (_absolute && element == "..")
            {
                if (stack.count() > 0)
                {
                    stack.pop_back();
                }
            }
            else if (element != ".")
            {
                stack.push_back(element);
            }

            return Iteration::CONTINUE;
        });

        return {_absolute, move(stack)};
    }

    String basename()
    {
        if (length() > 0)
        {
            return _elements.peek_back();
        }
        else
        {
            if (_absolute)
            {
                return "/";
            }
            else
            {
                return "";
            }
        }
    }

    String basename_without_extension()
    {
        StringBuilder builder{basename().length()};

        Lexer lexer{basename().cstring(), basename().length()};

        // It's not a file extention it's an hidden file.
        if (lexer.current_is("."))
        {
            builder.append(lexer.current());
            lexer.foreward();
        }

        while (!lexer.current_is(".") && lexer.do_continue())
        {
            builder.append(lexer.current());
            lexer.foreward();
        }

        return builder.finalize();
    }

    String dirname()
    {
        StringBuilder builder{};

        if (_absolute)
        {
            builder.append(PATH_SEPARATOR);
        }
        else if (_elements.count() <= 1)
        {
            builder.append(".");
        }

        if (_elements.count() >= 2)
        {
            for (size_t i = 0; i < _elements.count() - 1; i++)
            {
                builder.append(_elements[i]);

                if (i != _elements.count() - 2)
                {
                    builder.append(PATH_SEPARATOR);
                }
            }
        }

        return builder.finalize();
    }

    Path dirpath()
    {
        Vector<String> stack{};

        if (length() > 0)
        {
            for (size_t i = 0; i < length() - 1; i++)
            {
                stack.push_back(_elements[i]);
            }
        }

        return {_absolute, move(stack)};
    }

    String extension()
    {
        StringBuilder builder{basename().length()};

        Lexer lexer{basename().cstring(), basename().length()};

        // It's not a file extention it's an hidden file.
        if (lexer.current_is("."))
        {
            lexer.foreward();
        }

        while (!lexer.current_is("."))
        {
            lexer.foreward();
        }

        while (lexer.do_continue())
        {
            builder.append(lexer.current());
        }

        return builder.finalize();
    }

    String string()
    {
        StringBuilder builder{};

        if (_absolute)
        {
            builder.append(PATH_SEPARATOR);
        }

        for (size_t i = 0; i < _elements.count(); i++)
        {
            builder.append(_elements[i]);

            if (i != _elements.count() - 1)
            {
                builder.append(PATH_SEPARATOR);
            }
        }

        return builder.finalize();
    }
};

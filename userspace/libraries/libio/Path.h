#pragma once

#include <abi/Filesystem.h>
#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>
#include <libio/Scanner.h>
#include <libio/Write.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

namespace IO
{
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

    static Path parse(const String &string, int flags = 0)
    {
        return parse(string.cstring(), string.length(), flags);
    }

    static Path parse(const char *path, int flags = 0)
    {
        return parse(path, strlen(path), flags);
    }

    static Path parse(const char *path, size_t size, int flags)
    {
        IO::MemoryReader memory{path, size};
        IO::Scanner scan{memory};

        bool absolute = false;

        if (scan.skip(PATH_SEPARATOR))
        {
            absolute = true;
        }

        auto parse_element = [](auto &scan) {
            IO::MemoryWriter memory;

            while (!scan.skip(PATH_SEPARATOR) &&
                   !scan.ended())
            {
                IO::write(memory, scan.next());
            }

            return memory.string();
        };

        auto parse_shorthand = [](auto &scan) {
            Vector<String> elements{};

            scan.skip_word("..");
            elements.push("..");

            while (scan.skip('.'))
            {
                elements.push("..");
            }

            scan.skip('/');

            return elements;
        };

        Vector<String> elements{};

        while (!scan.ended())
        {
            if ((flags & PARENT_SHORTHAND) && scan.peek_is_word(".."))
            {
                elements.push_back_many(parse_shorthand(scan));
            }
            else
            {
                auto el = parse_element(scan);

                if (el->size() > 0)
                {
                    elements.push_back(el);
                }
            }
        }

        return {absolute, std::move(elements)};
    }

    static Path join(String left, String right)
    {
        return join(parse(left), parse(right));
    }

    static Path join(Path &&left, String right)
    {
        return join(left, parse(right));
    }

    static Path join(String left, Path &&right)
    {
        return join(parse(left), right);
    }

    static Path join(const Path &left, String right)
    {
        return join(left, parse(right));
    }

    static Path join(String left, Path &right)
    {
        return join(parse(left), right);
    }

    static Path join(Path &&left, Path &&right)
    {
        return join(left, right);
    }

    static Path join(Path &left, Path &&right)
    {
        return join(left, right);
    }

    static Path join(Path &&left, Path &right)
    {
        return join(left, right);
    }

    static Path join(const Path &left, const Path &right)
    {
        Vector<String> combined_elements{};

        combined_elements.push_back_many(left._elements);
        combined_elements.push_back_many(right._elements);

        return {left.absolute(), std::move(combined_elements)};
    }

    Path()
    {
    }

    Path(const Path &other) : _absolute{other.absolute()}, _elements{other._elements}
    {
    }

    Path(bool absolute, Vector<String> &&elements) : _absolute(absolute), _elements(elements)
    {
    }

    Path(Path &&other)
    {
        std::swap(_absolute, other._absolute);
        std::swap(_elements, other._elements);
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
            std::swap(_absolute, other._absolute);
            std::swap(_elements, other._elements);
        }

        return *this;
    }

    String operator[](size_t index) const
    {
        return _elements[index];
    }

    bool operator!=(const Path &other) const
    {
        return !(*this == other);
    }

    bool operator==(const Path &other) const
    {
        if (this == &other)
        {
            return true;
        }

        if (_absolute != other._absolute)
        {
            return false;
        }

        return _elements == other._elements;
    }

    Path normalized()
    {
        Vector<String> stack{};

        _elements.foreach([&](auto &element) {
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

        return {_absolute, std::move(stack)};
    }

    String basename() const
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

    String basename_without_extension() const
    {
        IO::MemoryWriter builder{basename().length()};
        IO::MemoryReader reader{basename().slice()};
        IO::Scanner scan{reader};

        // It's not a file extention it's an hidden file.
        if (scan.peek() == '.')
        {
            IO::write(builder, scan.next());
        }

        while (scan.peek() != '.' &&
               !scan.ended())
        {
            IO::write(builder, scan.next());
        }

        return builder.string();
    }

    String dirname() const
    {
        IO::MemoryWriter builder{};

        if (_absolute)
        {
            IO::write(builder, PATH_SEPARATOR);
        }
        else if (_elements.count() <= 1)
        {
            IO::write(builder, '.');
        }

        if (_elements.count() >= 2)
        {
            for (size_t i = 0; i < _elements.count() - 1; i++)
            {
                IO::write(builder, _elements[i]);

                if (i != _elements.count() - 2)
                {
                    IO::write(builder, PATH_SEPARATOR);
                }
            }
        }

        return builder.string();
    }

    Path dirpath() const
    {
        Vector<String> stack{};

        if (length() > 0)
        {
            for (size_t i = 0; i < length() - 1; i++)
            {
                stack.push_back(_elements[i]);
            }
        }

        return {_absolute, std::move(stack)};
    }

    String extension() const
    {
        auto filename = basename();

        IO::MemoryWriter builder{basename().length()};
        IO::MemoryReader reader{basename().slice()};
        IO::Scanner scan{reader};

        // It's not a file extention it's an hidden file.
        scan.skip('.');

        while (scan.peek() != '.' &&
               !scan.ended())
        {
            scan.next();
        }

        while (!scan.ended())
        {
            IO::write(builder, scan.next());
        }

        return builder.string();
    }

    Path parent(size_t index) const
    {
        Vector<String> stack{};

        if (index <= length())
        {
            for (size_t i = 0; i <= index; i++)
            {
                stack.push_back(_elements[i]);
            }
        }

        return {_absolute, std::move(stack)};
    }

    String string() const
    {
        IO::MemoryWriter builder{basename().length()};

        if (_absolute)
        {
            IO::write(builder, PATH_SEPARATOR);
        }

        for (size_t i = 0; i < _elements.count(); i++)
        {
            IO::write(builder, _elements[i]);

            if (i != _elements.count() - 1)
            {
                IO::write(builder, PATH_SEPARATOR);
            }
        }

        return builder.string();
    }
};

} // namespace IO
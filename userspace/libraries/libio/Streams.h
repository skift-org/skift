#pragma once

#include <libio/BufLine.h>
#include <libio/Format.h>
#include <libio/Handle.h>

#include <libsystem/process/Process.h>

namespace IO
{

struct InStream :
    public Reader,
    public RawHandle
{
private:
    RefPtr<Handle> _handle;

public:
    using Reader::read;

    InStream() : _handle{make<Handle>(0)} {}

    ResultOr<size_t> read(void *buffer, size_t size) override { return _handle->read(buffer, size); }
    RefPtr<Handle> handle() override { return _handle; }
};

struct OutStream :
    public Writer,
    public RawHandle
{
private:
    RefPtr<Handle> _handle;

public:
    using Writer::write;

    OutStream() : _handle{make<Handle>(1)} {}

    ResultOr<size_t> write(const void *buffer, size_t size) override { return _handle->write(buffer, size); }
    RefPtr<Handle> handle() override { return _handle; }
};

struct ErrStream :
    public Writer,
    public RawHandle

{
private:
    RefPtr<Handle> _handle;

public:
    using Writer::write;

    ErrStream() : _handle{make<Handle>(2)} {}

    ResultOr<size_t> write(const void *buffer, size_t size) override { return _handle->write(buffer, size); }
    RefPtr<Handle> handle() override { return _handle; }
};

struct LogStream :
    public Writer,
    public RawHandle
{
private:
    RefPtr<Handle> _handle;

public:
    using Writer::write;

    LogStream() : _handle{make<Handle>(3)} {}

    ResultOr<size_t> write(const void *buffer, size_t size) override { return _handle->write(buffer, size); }
    RefPtr<Handle> handle() override { return _handle; }
};

InStream &in();

OutStream &out();

ErrStream &err();

LogStream &log();

static inline ResultOr<String> inln()
{
    Scanner scan{in()};
    MemoryWriter writer{};

    while (!(scan.ended() || scan.peek() == '\n'))
    {
        writer.write(scan.peek());
        IO::write(out(), scan.peek());
        scan.next();
    }

    IO::write(out(), '\n');
    scan.skip('\n');

    return String{writer.string()};
}

template <typename... Args>
static ResultOr<size_t> print(Writer &writer, const char *fmt, Args... args)
{
    MemoryReader memory{fmt};
    Scanner scan{memory};
    return format(writer, scan, std::forward<Args>(args)...);
}

template <typename... Args>
static ResultOr<size_t> println(Writer &writer, const char *fmt, Args... args)
{
    MemoryReader memory{fmt};
    Scanner scan{memory};
    size_t written = TRY(format(writer, scan, std::forward<Args>(args)...));
    return written + TRY(IO::write(writer, '\n'));
}

template <typename... Args>
static ResultOr<size_t> out(const char *fmt, Args... args) { return print(out(), fmt, std::forward<Args>(args)...); }

template <typename... Args>
static ResultOr<size_t> outln(const char *fmt, Args... args) { return println(out(), fmt, std::forward<Args>(args)...); }

template <typename... Args>
static ResultOr<size_t> err(const char *fmt, Args... args) { return print(err(), fmt, std::forward<Args>(args)...); }

template <typename... Args>
static ResultOr<size_t> errln(const char *fmt, Args... args) { return println(err(), fmt, std::forward<Args>(args)...); }

template <typename... Args>
static ResultOr<size_t> log(const char *fmt, Args... args) { return print(log(), fmt, std::forward<Args>(args)...); }

template <typename... Args>
static ResultOr<size_t> logln(const char *fmt, Args... args)
{
    IO::BufLine buf{log()};

    size_t written = 0;

    written = TRY(print(buf, "\e[{}m{}({}) \e[37m", (process_this() % 6) + 91, process_name(), process_this()));
    written += TRY(print(buf, fmt, std::forward<Args>(args)...));
    written += TRY(IO::write(buf, "\e[m\n"));

    return written;
}

template <typename... Args>
static ResultOr<size_t> panicln(const char *fmt, Args... args)
{
    IO::BufLine buf{log()};

    print(buf, "\e[97;101m{}({}) ", process_name(), process_this());
    print(buf, fmt, std::forward<Args>(args)...);
    IO::write(buf, "\e[m\n");
    process_abort();
}

} // namespace IO

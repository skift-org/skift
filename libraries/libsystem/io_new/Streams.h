#pragma once

#include <libutils/String.h>

#include <libsystem/Handle.h>
#include <libsystem/io_new/Format.h>
#include <libsystem/io_new/MemoryWriter.h>
#include <libsystem/io_new/Reader.h>
#include <libsystem/io_new/Scanner.h>
#include <libsystem/io_new/Writer.h>

namespace System
{

class InStream :
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

class OutStream :
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

class ErrStream :
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

class LogStream :
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
    Scanner<Reader &> scan{in()};
    MemoryWriter writer{};

    while (!(scan.ended() || scan.current() == '\n'))
    {
        writer.write(scan.current());
        out().write(scan.current());
        scan.foreward();
    }

    out().write('\n');
    scan.skip('\n');

    return String{writer.string()};
}

template <Formatable... Args>
static ResultOr<size_t> print(Writer &writer, const char *fmt, Args... args)
{
    StringScanner scan{fmt, strlen(fmt)};
    return format(writer, scan, forward<Args>(args)...);
}

template <Formatable... Args>
static ResultOr<size_t> println(Writer &writer, const char *fmt, Args... args)
{
    StringScanner scan{fmt, strlen(fmt)};
    size_t written = TRY(format(writer, scan, forward<Args>(args)...));
    return written + TRY(writer.write('\n'));
}

template <Formatable... Args>
static ResultOr<size_t> out(const char *fmt, Args... args) { return print(out(), fmt, forward<Args>(args)...); }

template <Formatable... Args>
static ResultOr<size_t> outln(const char *fmt, Args... args) { return println(out(), fmt, forward<Args>(args)...); }

template <Formatable... Args>
static ResultOr<size_t> err(const char *fmt, Args... args) { return print(err(), fmt, forward<Args>(args)...); }

template <Formatable... Args>
static ResultOr<size_t> errln(const char *fmt, Args... args) { return println(err(), fmt, forward<Args>(args)...); }

template <Formatable... Args>
static ResultOr<size_t> log(const char *fmt, Args... args) { return print(log(), fmt, forward<Args>(args)...); }

template <Formatable... Args>
static ResultOr<size_t> logln(const char *fmt, Args... args) { return println(log(), fmt, forward<Args>(args)...); }

} // namespace System

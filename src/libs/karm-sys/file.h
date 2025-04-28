#pragma once

#include <karm-base/rc.h>

#include "async.h"
#include "fd.h"

namespace Karm::Sys {

struct FileReader;
struct FileWriter;

struct _File :
    Io::Seeker,
    Io::Flusher,
    Meta::NoCopy {

    Rc<Fd> _fd;
    Mime::Url _url;

    _File(Rc<Fd> fd, Mime::Url url)
        : _fd(fd), _url(url) {}

    Res<usize> seek(Io::Seek seek) override {
        return _fd->seek(seek);
    }

    Res<> flush() override {
        return _fd->flush();
    }

    auto flushAsync(auto& sched = globalSched()) {
        return sched.flushAsync(_fd);
    }

    Res<Stat> stat() {
        return _fd->stat();
    }

    Rc<Fd> fd() {
        return _fd;
    }
};

struct FileReader :
    virtual _File,
    Io::Reader {

    using _File::_File;

    Res<usize> read(MutBytes bytes) override {
        return _fd->read(bytes);
    }

    [[clang::coro_wrapper]]
    Async::Task<usize> readAsync(MutBytes bytes, Sched& sched = globalSched()) {
        return sched.readAsync(_fd, bytes);
    }
};

struct FileWriter :
    virtual _File,
    Io::Writer {

    using _File::_File;

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    [[clang::coro_wrapper]]
    Async::Task<usize> writeAsync(Bytes bytes, Sched& sched = globalSched()) {
        return sched.writeAsync(_fd, bytes);
    }
};

struct File :
    FileReader,
    FileWriter {

    using FileReader::FileReader;
    using FileWriter::FileWriter;

    static Res<FileWriter> create(Mime::Url url);

    static Res<FileReader> open(Mime::Url url);

    static Res<File> openOrCreate(Mime::Url url);
};

/// Read the entire file as a UTF-8 string.
static inline Res<String> readAllUtf8(Mime::Url const& url) {
    auto file = try$(Sys::File::open(url));
    return Io::readAllUtf8(file);
}

} // namespace Karm::Sys

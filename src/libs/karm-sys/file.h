#pragma once

#include <karm-base/rc.h>

#include "async.h"
#include "fd.h"

namespace Karm::Sys {

struct FileReader;
struct FileWriter;

struct _File :
    public Io::Seeker,
    public Io::Flusher,
    Meta::NoCopy {

    Strong<Fd> _fd;
    Mime::Url _url;

    _File(Strong<Fd> fd, Mime::Url url)
        : _fd(fd), _url(url) {}

    Res<usize> seek(Io::Seek seek) override {
        return _fd->seek(seek);
    }

    Res<usize> flush() override {
        return _fd->flush();
    }

    auto flushAsync(auto &sched = globalSched()) {
        return sched.flushAsync(_fd);
    }

    Res<Stat> stat() {
        return _fd->stat();
    }

    Strong<Fd> fd() {
        return _fd;
    }
};

struct FileReader :
    public virtual _File,
    public Io::Reader {

    using _File::_File;

    Res<usize> read(MutBytes bytes) override {
        return _fd->read(bytes);
    }

    auto readAsync(MutBytes bytes, auto &sched = globalSched()) {
        return sched.readAsync(_fd, bytes);
    }
};

struct FileWriter :
    public virtual _File,
    public Io::Writer {

    using _File::_File;

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    auto writeAsync(Bytes bytes, auto &sched = globalSched()) {
        return sched.writeAsync(_fd, bytes);
    }
};

struct File :
    public FileReader,
    public FileWriter {

    using FileReader::FileReader;
    using FileWriter::FileWriter;

    static Res<FileWriter> create(Mime::Url url);

    static Res<FileReader> open(Mime::Url url);

    static Res<File> openOrCreate(Mime::Url url);
};

/// Read the entire file as a UTF-8 string.
static inline Res<String> readAllUtf8(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    return Io::readAllUtf8(file);
}

} // namespace Karm::Sys

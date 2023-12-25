#pragma once

#include <karm-base/rc.h>

#include "fd.h"

namespace Karm::Sys {

struct FileReader;
struct FileWriter;

struct _File :
    public Io::Seeker,
    public Io::Flusher,
    Meta::NoCopy {

    Strong<Fd> _fd;
    Url::Url _url;

    _File(Strong<Fd> fd, Url::Url url)
        : _fd(fd), _url(url) {}

    Res<usize> seek(Io::Seek seek) override {
        return _fd->seek(seek);
    }

    Res<usize> flush() override {
        return _fd->flush();
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
};

struct FileWriter :
    public virtual _File,
    public Io::Writer {

    using _File::_File;

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }
};

struct File :
    public FileReader,
    public FileWriter {

    using FileReader::FileReader;
    using FileWriter::FileWriter;

    static Res<FileWriter> create(Url::Url url);

    static Res<FileReader> open(Url::Url url);

    static Res<File> openOrCreate(Url::Url url);
};

} // namespace Karm::Sys

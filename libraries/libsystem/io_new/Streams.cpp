#include <libsystem/io_new/Streams.h>

namespace System
{

static InStream _in{};
static OutStream _out{};
static ErrStream _err{};
static LogStream _log{};

InStream &in() { return _in; }

OutStream &out() { return _out; }

ErrStream &err() { return _err; }

LogStream &log() { return _log; }

} // namespace System
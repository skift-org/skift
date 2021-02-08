#include <libsystem/io_new/File.h>
#include <libsystem/io_new/Streams.h>

namespace System
{

File _in{Handle{0}};
File _out{Handle{1}};
File _err{Handle{2}};
File _log{Handle{3}};

Reader &in() { return _in; }

Writer &out() { return _out; }

Writer &err() { return _err; }

Writer &log() { return _log; }

} // namespace System
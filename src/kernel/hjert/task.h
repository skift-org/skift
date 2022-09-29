#pragma once

#include <hal/pmm.h>
#include <hal/vmm.h>
#include <karm-base/lock.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>

namespace Hjert {

struct _Object {
    uint64_t _id;
};

template <typename Crtp>
struct Object : public _Object {};

struct Vmo : public Object<Vmo> {
    Hal::PmmRange _range;
};

struct Mmap {
    Hal::VmmRange _range;
    Strong<Vmo> _vmo;
};

struct Space : public Object<Space> {
    Lock _lock;
    Vec<Mmap> _mmaps;
    Strong<Hal::Vmm> _vmm;

    Result<Hal::VmmRange> map(Strong<Vmo>);

    Result<Hal::VmmRange> unmap(Hal::VmmRange);
};

struct Domain : public Object<Domain> {
};

struct Channel : public Object<Channel> {
};

struct Context : public Object<Context> {
};

struct Task : public Object<Task> {
    bool _inSyscall;
    bool _isStopped;
    bool _isBlocked;
    bool _isStarted;

    size_t _timeStart;
    size_t _timeEnd;

    Strong<Context> _context;
    Strong<Space> _space;
    Strong<Domain> _domain;
    Strong<Channel> _channel;
};

} // namespace Hjert

#include <abi/Paths.h>

#include <libsystem/thread/Atomic.h>
#include <libsystem/utils/RingBuffer.h>

#include "arch/x86/COM.h"
#include "arch/x86/x86.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/interrupts/Dispatcher.h"

/* --- Serial device  node -------------------------------------------------- */

static RingBuffer *serial_buffer;

void serial_interrupt_handler()
{
    char byte = com_getc(COM1);

    ringbuffer_write(serial_buffer, (const char *)&byte, sizeof(byte));
}

class Serial : public FsNode
{
private:
    /* data */
public:
    Serial() : FsNode(FILE_TYPE_DEVICE)
    {
    }

    bool can_read(FsHandle *handle)
    {
        __unused(handle);

        // FIXME: make this atomic or something...
        return !ringbuffer_is_empty(serial_buffer);
    }

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size)
    {
        __unused(handle);

        // FIXME: use locks
        atomic_begin();
        size_t read = ringbuffer_read(serial_buffer, (char *)buffer, size);
        atomic_end();

        return read;
    }

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size)
    {
        __unused(handle);

        atomic_begin();
        size_t written = com_write(COM1, buffer, size);
        atomic_end();

        return written;
    }
};

void serial_initialize()
{
    serial_buffer = ringbuffer_create(1024);

    dispatcher_register_handler(4, serial_interrupt_handler);

    filesystem_link_and_take_ref_cstring(SERIAL_DEVICE_PATH, new Serial());
}

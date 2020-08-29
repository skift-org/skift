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

bool serial_can_read(FsNode *node, FsHandle *handle)
{
    __unused(node);
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(serial_buffer);
}

static Result serial_read(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);
    __unused(handle);

    // FIXME: use locks
    atomic_begin();
    *read = ringbuffer_read(serial_buffer, (char *)buffer, size);
    atomic_end();

    return SUCCESS;
}

static Result serial_write(FsNode *node, FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    __unused(node);
    __unused(handle);

    *written = com_write(COM1, buffer, size);

    return SUCCESS;
}

class Serial : public FsNode
{
private:
    /* data */
public:
    Serial() : FsNode(FILE_TYPE_DEVICE)
    {
        can_read = (FsNodeCanReadCallback)serial_can_read;
        read = (FsNodeReadCallback)serial_read;
        write = (FsNodeWriteCallback)serial_write;
    }

    ~Serial() {}
};

void serial_initialize()
{
    serial_buffer = ringbuffer_create(1024);

    dispatcher_register_handler(4, serial_interrupt_handler);

    filesystem_link_and_take_ref_cstring(SERIAL_DEVICE_PATH, new Serial());
}

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/atomic.h>

#include "filesystem/Filesystem.h"
#include "processor.h"
#include "system.h"

#include "serial.h"

#define PORT_COM1 0x3f8

static void wait_read()
{
    while ((in8(PORT_COM1 + 5) & 1) == 0)
    { /* do nothing */
    }
}

static void wait_write()
{
    while ((in8(PORT_COM1 + 5) & 0x20) == 0)
    { /* do nothing */
    }
}

char serial_getc()
{
    wait_read();
    return in8(PORT_COM1);
}

void serial_putc(char c)
{
    wait_write();
    out8(PORT_COM1, c);
}

int serial_read(char *buffer, uint size)
{
    for (uint i = 0; i < size; i++)
    {
        buffer[i] = serial_getc();

        if (buffer[i] == '\n')
        {
            return i + 1;
        }
    }

    return size;
}

int serial_write(const char *buffer, uint size)
{
    atomic_begin();

    for (uint i = 0; i < size; i++)
    {
        serial_putc(buffer[i]);
    }

    atomic_end();

    return size;
}

/* --- Serial device  node -------------------------------------------------- */

static error_t serial_FsOperationRead(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *readed)
{
    __unused(node);
    __unused(handle);

    *readed = serial_read(buffer, size);

    return ERR_SUCCESS;
}

static error_t serial_FsOperationWrite(FsNode *node, FsHandle *handle, const void *buffer, size_t size, size_t *writen)
{
    __unused(node);
    __unused(handle);

    *writen = serial_write(buffer, size);

    return ERR_SUCCESS;
}

void serial_initialize(void)
{
    // See: https://wiki.osdev.org/Serial_Ports
    out8(PORT_COM1 + 1, 0x00);
    out8(PORT_COM1 + 3, 0x80);
    out8(PORT_COM1 + 0, 0x03);
    out8(PORT_COM1 + 1, 0x00);
    out8(PORT_COM1 + 3, 0x03);
    out8(PORT_COM1 + 2, 0xC7);
    out8(PORT_COM1 + 4, 0x0B);

    FsNode *serial_device = __create(FsNode);
    fsnode_init(serial_device, FSNODE_DEVICE);

    FSNODE(serial_device)->read = (FsOperationRead)serial_FsOperationRead;
    FSNODE(serial_device)->write = (FsOperationWrite)serial_FsOperationWrite;

    Path *serial_device_path = path("/dev/serial");
    filesystem_link_and_take_ref(serial_device_path, serial_device);
    path_delete(serial_device_path);
}
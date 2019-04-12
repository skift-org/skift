#include <skift/__plugs__.h>

#include <skift/iostream.h>

#include "kernel/sheduler.h"
#include "kernel/process.h"

int __plug_iostream_open(const char *file_path, iostream_flag_t flags)
{
    return process_open_file(sheduler_running_process(), file_path, flags);
}

int __plug_iostream_close(int fd)
{
    return process_close_file(sheduler_running_process(), fd);
}

int __plug_iostream_read(int fd, void *buffer, uint size)
{
    return process_read_file(sheduler_running_process(), fd, buffer, size);
}

int __plug_iostream_write(int fd, void *buffer, uint size)
{
    return process_write_file(sheduler_running_process(), fd, buffer, size);
}

int __plug_iostream_ioctl(int fd, int request, void *args)
{
    return process_ioctl_file(sheduler_running_process(), fd, request, args);
}

int __plug_iostream_seek(int fd, int offset, iostream_whence_t whence)
{
    return process_seek_file(sheduler_running_process(), fd, offset, whence);
}

int __plug_iostream_tell(int fd, iostream_whence_t whence)
{
    return process_tell_file(sheduler_running_process(), fd, whence);
}

int __plug_iostream_fstat(int fd, iostream_stat_t *stat)
{
    return process_fstat_file(sheduler_running_process(), fd, stat);
}
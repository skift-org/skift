#include <skift/__plugs__.h>

#include <skift/iostream.h>
#include <skift/syscalls.h>

iostream_t *in_stream ;
iostream_t *out_stream;
iostream_t *err_stream;
iostream_t *log_stream;

void __plug_iostream_init(void)
{
    in_stream  = NULL;
    out_stream = iostream_open("/dev/console", IOSTREAM_WRITE | IOSTREAM_BUFFERED_WRITE);
    err_stream = NULL;
    log_stream = iostream_open("/dev/serial", IOSTREAM_WRITE | IOSTREAM_BUFFERED_WRITE);
}

int __plug_iostream_open(const char *path, iostream_flag_t flags)
{
    return __syscall(SYS_FILESYSTEM_OPEN, (int)path, flags, 0, 0, 0);
}

int __plug_iostream_close(int fd)
{
    return __syscall(SYS_FILESYSTEM_CLOSE, fd, 0, 0, 0, 0);
}

int __plug_iostream_read(int fd, void *buffer, uint size)
{
    return __syscall(SYS_FILESYSTEM_READ, fd, (int)buffer, size, 0, 0);
}

int __plug_iostream_write(int fd, const void *buffer, uint size)
{
    return __syscall(SYS_FILESYSTEM_WRITE, fd, (int)buffer, size, 0, 0);
}

int __plug_iostream_ioctl(int fd, int request, void *args)
{
    return __syscall(SYS_FILESYSTEM_IOCTL, fd, request, (int)args, 0, 0);
}

int __plug_iostream_seek(int fd, int offset, iostream_whence_t whence)
{
    return __syscall(SYS_FILESYSTEM_SEEK, fd, offset, whence, 0, 0);
}

int __plug_iostream_tell(int fd, iostream_whence_t whence)
{
    return __syscall(SYS_FILESYSTEM_TELL, fd, whence, 0, 0, 0);
}

int __plug_iostream_fstat(int fd, iostream_stat_t *stat)
{
    return __syscall(SYS_FILESYSTEM_FSTAT, fd, (int)stat, 0, 0, 0);
}
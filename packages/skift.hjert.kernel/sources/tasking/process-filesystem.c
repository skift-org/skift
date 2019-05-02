#include <skift/path.h>
#include <skift/iostream.h>

#include "kernel/filesystem.h"
#include "kernel/process.h"

void process_filedescriptor_close_all(process_t* this)
{
    for(int i = 0; i < MAX_PROCESS_OPENED_FILES; i++)
    {
        process_close_file(this, i);
    }
}

// File descriptor allocation and locking ----------------------------------- //

int process_filedescriptor_alloc_and_acquire(process_t *this, stream_t *stream) 
{
    sk_lock_acquire(this->fds_lock);

    for(int i = 0; i < MAX_PROCESS_OPENED_FILES; i++)
    {
        process_filedescriptor_t *fd = &this->fds[i];

        if (fd->free)
        {
            fd->free = false;
            fd->stream = stream;
            sk_lock_acquire(fd->lock);

            sk_lock_release(this->fds_lock);

            sk_log(LOG_DEBUG, "File descriptor %d allocated for process %d'%s'", i, this->id, this->name);

            return i;
        }
    }
    
    sk_lock_release(this->fds_lock);
    sk_log(LOG_WARNING, "We run out of file descriptor on process %d'%s'", this->id, this->name);

    return -1;
}

stream_t *process_filedescriptor_acquire(process_t *this, int fd_index) 
{
    if (fd_index >= 0 && fd_index < MAX_PROCESS_OPENED_FILES)
    {
        process_filedescriptor_t *fd = &this->fds[fd_index];
        sk_lock_acquire(fd->lock);

        if (!fd->free)
        {
            return fd->stream;
        }
    }

    sk_log(LOG_WARNING, "Got a bad file descriptor %d from process %d'%s'", fd_index, this->id, this->name);

    return NULL;
}

int process_filedescriptor_release(process_t *this, int fd_index) 
{
    if (fd_index >= 0 && fd_index < MAX_PROCESS_OPENED_FILES)
    {
        process_filedescriptor_t *fd = &this->fds[fd_index];

        sk_lock_release(fd->lock);

        return 0;
    }

    sk_log(LOG_WARNING, "Got a bad file descriptor %d from process %d'%s'", fd_index, this->id, this->name);

    return -1;
}

int process_filedescriptor_free_and_release(process_t *this, int fd_index) 
{
    if (fd_index >= 0 && fd_index < MAX_PROCESS_OPENED_FILES)
    {
        process_filedescriptor_t *fd = &this->fds[fd_index];

        sk_lock_release(fd->lock);

        fd->free = true;
        fd->stream = NULL;

        sk_log(LOG_DEBUG, "File descriptor %d free for process %d'%s'", fd_index, this->id, this->name);

        return 0;
    }

    sk_log(LOG_WARNING, "Got a bad file descriptor %d from process %d'%s'", fd_index, this->id, this->name);

    return -1;
}

// Process file operations -------------------------------------------------- //

int process_open_file(process_t* this, const char *file_path, iostream_flag_t flags)
{
    path_t *p = path(file_path);
    stream_t *stream = filesystem_open(NULL, p, flags);

    if (stream == NULL)
    {
        return -1;
    }

    int fd = process_filedescriptor_alloc_and_acquire(this, stream);

    if (fd == -1)
    {
        return -1;
    }

    process_filedescriptor_release(this, fd);

    return fd;
}

int process_close_file(process_t* this, int fd)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return -1;
    }

    filesystem_close(stream);

    process_filedescriptor_free_and_release(this, fd);

    return 0;
}

int process_read_file(process_t* this, int fd, void *buffer, uint size)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_read(stream, buffer, size);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_write_file(process_t* this, int fd, const void *buffer, uint size)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_write(stream, buffer, size);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_ioctl_file(process_t* this, int fd, int request, void *args)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_ioctl(stream, request, args);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_seek_file(process_t* this, int fd, int offset, iostream_whence_t whence)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_seek(stream, offset, whence);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_tell_file(process_t* this, int fd, iostream_whence_t whence)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_tell(stream, whence);

    process_filedescriptor_release(this, fd);

    return result;
}

int process_fstat_file(process_t* this, int fd, iostream_stat_t *stat)
{
    stream_t *stream = process_filedescriptor_acquire(this, fd);

    if (stream == NULL)
    {
        return 0;
    }

    int result = filesystem_fstat(stream, stat);

    process_filedescriptor_release(this, fd);

    return result;
}
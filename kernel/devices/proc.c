/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* proc.c : process list device                                               */

#include <skift/atomic.h>
#include <skift/math.h>

#include "devices.h"
#include "filesystem.h"
#include "tasking.h"

int proc_device_open(stream_t *stream)
{
    atomic_begin();

    list_t *tasks = task_all();
    task_info_t *infos = malloc(sizeof(task_info_t) * list_count(tasks));

    int index = 0;

    list_foreach(i, tasks)
    {
        task_t *task = i->value;

        task_get_info(task, &infos[index]);

        index++;
    }

    stream->attached.p = infos;
    stream->attached.count = list_count(tasks);
    stream->attached.current = 0;

    atomic_end();

    return 0;
}

int proc_device_close(stream_t *stream)
{
    free(stream->attached.p);

    return 0;
}

int proc_device_read(stream_t *s, void *buffer, uint size)
{
    if (s->attached.current < s->attached.count)
    {
        task_info_t *info = &((task_info_t *)s->attached.p)[s->attached.current];

        memcpy(buffer, info, min(size, sizeof(task_info_t)));

        s->attached.current++;

        return size;
    }
    else
    {
        return 0;
    }
}

int proc_device_write(stream_t *s, const void *buffer, uint size)
{
    UNUSED(s);
    UNUSED(buffer);
    UNUSED(size);

    return 0;
}

void proc_setup(void)
{
    device_t zero_device = {
        .open = proc_device_open,
        .close = proc_device_close,
        .read = proc_device_read,
        .write = proc_device_write,
    };

    FILESYSTEM_MKDEV("/dev/proc", zero_device);
}
#include <string.h>
#include <skift/atomic.h>
#include <skift/path.h>

#include "kernel/system.h"

#include "kernel/filesystem_new.h"


static fsnode_t* root;

#define ENSURE_TYPE(_t, _n) if (_n->type == _t) { panic("File type error, expected %s but get %s!", fsnode_type_str[_t], fsnode_type_str[_n->type]); }
#define UNREF(_n) ATOMIC({if (--_n->refcount == 0){ fsnode_delete(_n); }});
#define REF(_n)  ATOMIC({_n->refcount++;})

/* --- Constructor/destructor ----------------------------------------------- */

fsnode_t *fsnode(const char *name)
{
    fsnode_t *fn = MALLOC(fsnode_t);

    strncpy(fn->name, name, FSNAME_SIZE);

    return fn;
}

fsnode_t *file(const char* name)
{
    fsnode_t* n = fsnode(name);

    n->type = FSNODE_FILE;

    n->file.buffer = malloc(4096);
    n->file.size = 4096;
    n->file.head = 0;

    return n;
}

void file_delete(fsnode_t* file)
{
    ENSURE_TYPE(FSNODE_FILE, file);

    free(file->file.buffer);
}

fsnode_t *device(const char* name, device_t* dev)
{

}

void device_delete(fsnode_t * device)
{
    ENSURE_TYPE(FSNODE_DEVICE, device);
}

fsnode_t *link(const char name, fsnode_t* to)
{
    REF(to);
    fsnode_t* n = fsnode(name);
    n->link.to = to;
    return n;
}

void link_delete(fsnode_t *link)
{
    ENSURE_TYPE(FSNODE_LINK, link);
}

fsnode_t *directory(const char name)
{

}

void directory_delete(fsnode_t* dir)
{
    ENSURE_TYPE(FSNODE_DIRECTORY, dir);
}

void fsnode_delete(fsnode_t* fn)
{
    switch (fn->type)
    {
    case FSNODE_FILE: file_delete(fn);
        break;
    case FSNODE_DEVICE: device_delete(fn);
        break;
    case FSNODE_LINK: link_delete(fn);
        break;
    case FSNODE_DIRECTORY: directory_delete(fn);
        break;
    default: panic("Invalide fs node type:%d.", fn->type);
        break;
    }

    free(fn);
}

/* --- Private function ----------------------------------------------------- */

fsnode_t* resolve_path(fsnode_t* relative, const char *path)
{
    char atom[FSNAME_SIZE];
    fsnode_t *current = (relative ? relative : root);

    for(size_t i = 0; path_read(path, i, atom); i++)
    {
        if (current->type == FSNODE_LINK)
        {
            current = current->link.to;
        }

        if(current->type == FSNODE_DIRECTORY)
        {
            
        }
        else
        {
            return NULL;
        }
    }

    return current;
}

/* --- Public functions ----------------------------------------------------- */

void filesystem_setup(void)
{
    root = directory("ROOT");
}

fsnode_t* filesystem_open(fsnode_t* relative, const char *path, open_option_t option)
{

}

void filesystem_close(fsnode_t* node)
{

}

int filesystem_read(fsnode_t* node, void* buffer, uint size)
{

}

int filesystem_write(fsnode_t* node, void* buffer, uint size)
{

}

void filesystem_seek(fsnode_t* node, int offset, seek_origin_t origin)
{

}

int filesystem_tell(fsnode_t* node)
{

}

void filesystem_ioctl(fsnode_t* node, int request, void* args)
{

}

void filesystem_mkdir(fsnode_t* relative, const char *path)
{

}

bool filesystem_rm(fsnode_t* relative, const char *path)
{

}
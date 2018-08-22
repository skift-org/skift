#pragma once
#include "types.h"

typedef int DEVICE;

typedef bool (* open_t)  (void *);
typedef bool (* close_t) (void *);
typedef uint  (* umount_t)(void *);
typedef uint  (* read_t)  (void *, void *, u32, u32);
typedef uint  (* write_t) (void *, void *, u32, u32);
typedef uint  (* ioctl_t) (void *, u32, void *);

typedef struct
{
    string name;
    bool is_open;

    open_t open;
    close_t close;
    read_t read;
    write_t write;
    umount_t umount;
    ioctl_t func;

    void * data;
} device_t;


#define DEF_DEV_OPEN(name)   bool dev_##name##_open     (device_t * dev)
#define DEF_DEV_CLOSE(name)  bool dev_##name##_close    (device_t * dev)
#define DEF_DEV_READ(name)   u32  dev_##name##_read     (device_t * dev, void * buffer, u32 size, u32 offset)
#define DEF_DEV_WRITE(name)  u32  dev_##name##_write    (device_t * dev, void * buffer, u32 size, u32 offset)
#define DEF_DEV_UMOUNT(name) bool dev_##name##_umount   (device_t * dev)
#define DEF_DEV_IOCTL(name)  u32 dev_##name##_IOCTL     (device_t * dev, u32 command, void* argp)

#define DEV_OPEN(name)   (open_t)   dev_##name##_open
#define DEV_CLOSE(name)  (close_t)  dev_##name##_close
#define DEV_READ(name)   (read_t)   dev_##name##_read
#define DEV_WRITE(name)  (write_t)  dev_##name##_write
#define DEV_UMOUNT(name) (umount_t) dev_##name##_umount 
#define DEF_IOCTL(name)  (ioctl_t) dev_##name##_ioctl

bool mount_device (string name,
                  void * open,
                  void * close, 
                  void * read, 
                  void * write,
                  void * umount,
                  void * ioctl,
                  void * data);

device_t * device(string name);

DEVICE device_open  (string name);
void   device_close (DEVICE dev );
u32    device_read  (DEVICE dev, void * buffer, u32 size, s32 offset);
u32    device_write (DEVICE dev, void * buffer, u32 size, s32 offset);
bool   device_umount(string name);
u32    device_ioctl (DEVICE dev, u32 command, void *argp);
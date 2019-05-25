#include <hjert/graphic.h>
#include <hjert/filesystem.h>

#if 0
int graphic_device_ioctl(int op, void* arg)
{

}

/* --- Public functions ----------------------------------------------------- */

void graphic_early_setup(multiboot_info_t* mbootinfo)
{

}

void graphic_setup(void)
{
    device_t graphic_device = {

    };

    FILESYSTEM_MKDEV("graphic", graphic_device);
}
#endif


/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cmdline.h>
#include <libsystem/iostream.h>
#include <libsystem/logger.h>
#include <libsystem/process.h>
#include <libsystem/error.h>

#include <libdevice/keyboard.h>
#include <libdevice/keymap.h>

bool option_get;
char *option_set;
bool option_list;

static const char *usages[] = {
    "",
    "OPTION...",
    NULL,
};

static cmdline_option_t options[] = {
    CMDLINE_OPT_HELP,

    CMDLINE_OPT_BOOL("get", 'g', option_get, "Get the current keyboard keymap.", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_STRING("set", 's', option_set, "Set the keyboard keymap", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("list", 'l', option_list, "List all installed keymap on this system", CMDLINE_NO_CALLBACK),

    CMDLINE_OPT_END,
};

static cmdline_t cmdline = CMDLINE(usages, options, "Get or set the current keyboard keymap", "Options can be combined.");

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    // Default to option_get.
    if (option_get == false && option_set == NULL && option_list == false)
    {
        option_get = true;
    }

    iostream_t *keyboard_device = iostream_open(KEYBOARD_DEVICE, IOSTREAM_READ);

    if (keyboard_device == NULL)
    {
        error_print("Failled to open the keyboard device");

        return -1;
    }

    if (option_get)
    {
        keymap_t keymap;
        if (iostream_call(keyboard_device, FRAMEBUFFER_CALL_GET_KEYMAP, &keymap) == 0)
        {
            printf("Current keymap is %s(%s)", keymap.language, keymap.region);
        }
        else
        {
            printf("Failled to retrived the current keymap!\n");
        }
    }

    if (option_set != NULL)
    {
        logger_info("Loading keymap file from %s", option_set);
        iostream_t *keymap_file = iostream_open(option_set, IOSTREAM_READ);

        if (keymap_file != NULL)
        {
            iostream_stat_t stat;
            iostream_stat(keymap_file, &stat);

            logger_info("Allocating keymap of size %dkio", stat.size / 1024);
            keymap_t *new_keymap = malloc(stat.size);

            iostream_read(keymap_file, new_keymap, stat.size);

            iostream_close(keymap_file);

            if (new_keymap->magic[0] == 'k' && 
                new_keymap->magic[1] == 'm' && 
                new_keymap->magic[2] == 'a' && 
                new_keymap->magic[3] == 'p')
            {
                keyboard_set_keymap_args_t args = {.size = stat.size, .keymap = new_keymap};
                iostream_call(keyboard_device, FRAMEBUFFER_CALL_SET_KEYMAP, &args);
            }
            else
            {
                printf("Invalid file format!");
                iostream_close(keyboard_device);
           
                return -1;
            }
        }
        else
        {
            error_print("Failled to open the keymap file");
            iostream_close(keyboard_device);
            return -1;
        }
    }

    if (option_list)
    {
    }

    iostream_close(keyboard_device);

    return 0;
}

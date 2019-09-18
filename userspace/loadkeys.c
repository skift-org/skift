/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cmdline.h>
#include <libsystem/iostream.h>
#include <libsystem/logger.h>
#include <libsystem/process.h>
#include <libsystem/error.h>
#include <libsystem/cstring.h>

#include <libdevice/keyboard.h>
#include <libdevice/keymap.h>

bool option_get;
bool option_list;

static const char *usages[] = {
    "",
    "OPTION...",
    NULL,
};

static cmdline_option_t options[] = {
    CMDLINE_OPT_HELP,

    CMDLINE_OPT_BOOL("get", 'g', option_get, "Get the current keyboard keymap.", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("list", 'l', option_list, "List all installed keymap on this system", CMDLINE_NO_CALLBACK),

    CMDLINE_OPT_END,
};

static cmdline_t cmdline = CMDLINE(usages, options, "Get or set the current keyboard keymap", "Options can be combined.");

int loadkey_list_keymap()
{
    iostream_t* dir = iostream_open("/res/keyboard", IOSTREAM_READ);

    iostream_direntry_t entry = {0};
    while (iostream_read(dir, &entry, sizeof(entry)) > 0)
    {
        // FIXME: maybe show some info about the kmap file
        printf("- %s\n", entry.name);
    }

    iostream_close(dir);

    return 0;
}

int loadkey_set_keymap(iostream_t *keyboard_device, const char *keymap_path)
{
    logger_info("Loading keymap file from %s", keymap_path);
    iostream_t *keymap_file = iostream_open(keymap_path, IOSTREAM_READ);

    if (keymap_file != NULL)
    {
        iostream_stat_t stat;
        iostream_stat(keymap_file, &stat);

        logger_info("Allocating keymap of size %dkio", stat.size / 1024);

        if (stat.size < sizeof(keymap_t))
        {
            iostream_printf(err_stream, "Invalid keymap file format!\n");
            return -1;
        }

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

            printf("Keymap set to %s(%s)\n", new_keymap->language, new_keymap->region);

            free(new_keymap);

            return 0;
        }
        else
        {
            free(new_keymap);

            iostream_printf(err_stream, "Invalid keymap file format!\n");

            return -1;
        }
    }
    else
    {
        error_print("Failled to open the keymap file");

        return -1;
    }
}

int loadkey_get_keymap(iostream_t *keyboard_device)
{
    keymap_t keymap;

    if (iostream_call(keyboard_device, FRAMEBUFFER_CALL_GET_KEYMAP, &keymap) == 0)
    {
        printf("Current keymap is %s(%s)\n", keymap.language, keymap.region);

        return 0;
    }
    else
    {
        iostream_printf(err_stream, "Failled to retrived the current keymap!\n");

        return -1;
    }
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    iostream_t *keyboard_device = iostream_open(KEYBOARD_DEVICE, IOSTREAM_READ);

    if (keyboard_device == NULL)
    {
        error_print("Failled to open the keyboard device");

        return -1;
    }

    if (!option_list && argc == 1)
    {
        option_get = true;
    }

    if (option_get)
    {
        return loadkey_get_keymap(keyboard_device);
    }

    if (option_list)
    {
        return loadkey_list_keymap();
    }

    if (!option_get && !option_list && argc == 2)
    {
        char font_path[PATH_LENGHT] = {0};
        snprintf(font_path, PATH_LENGHT, "/res/keyboard/%s.kmap", argv[1]);
        return loadkey_set_keymap(keyboard_device, font_path);
    }

    iostream_close(keyboard_device);

    return 0;
}

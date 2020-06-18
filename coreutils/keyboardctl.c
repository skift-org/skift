#include <abi/Keyboard.h>
#include <abi/Paths.h>

#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

bool option_get;
bool option_list;

static const char *usages[] = {
    "",
    "OPTION...",
    NULL,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("get", 'g', option_get, "Get the current keyboard keymap.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("list", 'l', option_list, "List all installed keymap on this system", COMMANDLINE_NO_CALLBACK),

    COMMANDLINE_OPT_END,
};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Get or set the current keyboard keymap",
    "Options can be combined.");

int loadkey_list_keymap(void)
{
    Directory *keymap_directory = directory_open("/res/keyboard", OPEN_READ);

    if (handle_has_error(keymap_directory))
    {
        handle_printf_error(keymap_directory, "keyboardctl: Failled to query keymaps from /res/keyboard");
        directory_close(keymap_directory);

        return -1;
    }

    DirectoryEntry entry = {};
    while (directory_read(keymap_directory, &entry) > 0)
    {
        // FIXME: maybe show some info about the kmap file
        printf("- %s\n", entry.name);
    }

    directory_close(keymap_directory);

    return 0;
}

int loadkey_set_keymap(Stream *keyboard_device, const char *keymap_path)
{
    logger_info("Loading keymap file from %s", keymap_path);

    __cleanup(stream_cleanup) Stream *keymap_file = stream_open(keymap_path, OPEN_READ);

    if (handle_has_error(keymap_file))
    {
        handle_printf_error(keyboard_device, "keyboardctl: Failled to open the keymap file");

        return -1;
    }

    FileState stat;
    stream_stat(keymap_file, &stat);

    logger_info("Allocating keymap of size %dkio", stat.size / 1024);

    if (stat.size < sizeof(KeyMap))
    {
        stream_printf(err_stream, "keyboardctl: Invalid keymap file format!\n");
        return -1;
    }

    __cleanup_malloc KeyMap *new_keymap = (KeyMap *)malloc(stat.size);

    stream_read(keymap_file, new_keymap, stat.size);

    if (new_keymap->magic[0] != 'k' ||
        new_keymap->magic[1] != 'm' ||
        new_keymap->magic[2] != 'a' ||
        new_keymap->magic[3] != 'p')
    {
        stream_printf(err_stream, "keyboardctl: Invalid keymap file format!\n");
        return -1;
    }

    IOCallKeyboardSetKeymapArgs args = {.size = stat.size, .keymap = new_keymap};
    stream_call(keyboard_device, IOCALL_KEYBOARD_SET_KEYMAP, &args);

    printf("Keymap set to %s(%s)\n", new_keymap->language, new_keymap->region);

    return 0;
}

int loadkey_get_keymap(Stream *keyboard_device)
{
    KeyMap keymap;

    if (stream_call(keyboard_device, IOCALL_KEYBOARD_GET_KEYMAP, &keymap) != SUCCESS)
    {
        handle_printf_error(keyboard_device, "keyboardctl: Failled to retrived the current keymap");
        return -1;
    }

    printf("Current keymap is %s(%s)\n", keymap.language, keymap.region);
    return 0;
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    __cleanup(stream_cleanup) Stream *keyboard_device = stream_open(KEYBOARD_DEVICE_PATH, OPEN_READ);

    if (handle_has_error(keyboard_device))
    {
        handle_printf_error(keyboard_device, "keyboardctl: Failled to open the keyboard device");

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
        char font_path[PATH_LENGTH] = {};
        snprintf(font_path, PATH_LENGTH, "/res/keyboard/%s.kmap", argv[1]);

        return loadkey_set_keymap(keyboard_device, font_path);
    }

    return 0;
}

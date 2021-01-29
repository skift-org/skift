#include <abi/Keyboard.h>
#include <abi/Paths.h>

#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

bool option_get;
bool option_list;

static const char *usages[] = {
    "",
    "OPTION...",
    nullptr,
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

int loadkey_list_keymap()
{
    Directory *keymap_directory = directory_open("/Files/Keyboards", OPEN_READ);

    if (handle_has_error(keymap_directory))
    {
        handle_printf_error(keymap_directory, "keyboardctl: Failed to query keymaps from /Files/Keyboards");
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

    __cleanup_malloc KeyMap *keymap = nullptr;
    size_t keymap_size = 0;

    File file{keymap_path};
    auto result_or_keymapdata = file.read_all();

    if (!result_or_keymapdata.success())
    {
        stream_format(err_stream, "keyboardctl: Failed to open the keymap file: %s", result_or_keymapdata.description());

        return PROCESS_FAILURE;
    }

    if (keymap_size < sizeof(KeyMap) ||
        keymap->magic[0] != 'k' ||
        keymap->magic[1] != 'm' ||
        keymap->magic[2] != 'a' ||
        keymap->magic[3] != 'p')
    {
        stream_format(err_stream, "keyboardctl: Invalid keymap file format!\n");

        return PROCESS_FAILURE;
    }

    IOCallKeyboardSetKeymapArgs args = {
        .keymap = keymap,
        .size = keymap_size,
    };
    stream_call(keyboard_device, IOCALL_KEYBOARD_SET_KEYMAP, &args);

    printf("Keymap set to %s(%s)\n", keymap->language, keymap->region);

    return PROCESS_SUCCESS;
}

int loadkey_get_keymap(Stream *keyboard_device)
{
    KeyMap keymap;

    if (stream_call(keyboard_device, IOCALL_KEYBOARD_GET_KEYMAP, &keymap) != SUCCESS)
    {
        handle_printf_error(keyboard_device, "keyboardctl: Failed to retrived the current keymap");
        return PROCESS_FAILURE;
    }

    printf("Current keymap is %s(%s)\n", keymap.language, keymap.region);
    return PROCESS_SUCCESS;
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    __cleanup(stream_cleanup) Stream *keyboard_device = stream_open(KEYBOARD_DEVICE_PATH, OPEN_READ);

    if (handle_has_error(keyboard_device))
    {
        handle_printf_error(keyboard_device, "keyboardctl: Failed to open the keyboard device");

        return PROCESS_FAILURE;
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
        snprintf(font_path, PATH_LENGTH, "/Files/Keyboards/%s.kmap", argv[1]);

        return loadkey_set_keymap(keyboard_device, font_path);
    }

    return PROCESS_SUCCESS;
}

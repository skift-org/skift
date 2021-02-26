#include <abi/Keyboard.h>
#include <abi/Paths.h>

#include <libutils/ArgParse.h>

#include <libsystem/io_new/Copy.h>
#include <libsystem/io_new/Directory.h>
#include <libsystem/io_new/File.h>
#include <libsystem/io_new/Streams.h>

int loadkey_list_keymap()
{
    System::Directory keymap_directory{"/Files/Keyboards"};

    if (!keymap_directory.exist())
    {
        System::errln("keyboardctl: Failed to query keymaps from /Files/Keyboards");
        return PROCESS_FAILURE;
    }

    for (auto entry : keymap_directory.entries())
    {
        Path keymap_path = Path::parse(entry.name);
        System::outln("- {}", keymap_path.basename_without_extension());
    }

    return PROCESS_SUCCESS;
}

int loadkey_set_keymap(System::Handle &keyboard_device, String keymap_path)
{
    System::File file{keymap_path, OPEN_READ};
    auto read_all_result = System::read_all(file);

    if (!read_all_result.success())
    {
        System::errln("keyboardctl: Failed to open the keymap file: {}", read_all_result.description());

        return PROCESS_FAILURE;
    }

    KeyMap *keymap = (KeyMap *)read_all_result->start();
    size_t keymap_size = read_all_result->size();

    if (keymap_size < sizeof(KeyMap) ||
        keymap->magic[0] != 'k' ||
        keymap->magic[1] != 'm' ||
        keymap->magic[2] != 'a' ||
        keymap->magic[3] != 'p')
    {
        System::errln("keyboardctl: Invalid keymap file format!");

        return PROCESS_FAILURE;
    }

    IOCallKeyboardSetKeymapArgs args = {
        .keymap = keymap,
        .size = keymap_size,
    };

    auto call_result = keyboard_device.call(IOCALL_KEYBOARD_SET_KEYMAP, &args);

    if (call_result != SUCCESS)
    {
        System::errln("keyboardctl: Failed to open the keymap file: {}", get_result_description(call_result));

        return PROCESS_FAILURE;
    }

    System::outln("Keymap set to {}({})", keymap->language, keymap->region);

    return PROCESS_SUCCESS;
}

int loadkey_get_keymap(System::Handle &keyboard_device)
{
    KeyMap keymap;

    if (keyboard_device.call(IOCALL_KEYBOARD_GET_KEYMAP, &keymap) != SUCCESS)
    {
        System::errln("keyboardctl: Failed to retrived the current keymap");
        return PROCESS_FAILURE;
    }

    System::outln("Current keymap is {}({})", keymap.language, keymap.region);

    return PROCESS_SUCCESS;
}

int main(int argc, const char *argv[])
{
    ArgParse args{};
    args.should_abort_on_failure();
    args.show_help_if_no_option_given();

    args.prologue("Get or set the current keyboard keymap");

    args.usage("");
    args.usage("OPTION...");

    args.epiloge("Options can be combined.");

    System::Handle keyboard_handle{KEYBOARD_DEVICE_PATH, OPEN_READ};

    if (!keyboard_handle.valid())
    {
        System::errln("keyboardctl: Failed to open the keyboard device");

        return PROCESS_FAILURE;
    }

    args.option('l', "list", "List all installed keymap on this system.", [&](auto &) {
        return loadkey_list_keymap();
    });

    args.option('g', "get", "Get the current keyboard keymap.", [&](auto &) {
        return loadkey_get_keymap(keyboard_handle);
    });

    args.option_string('s', "set", "Set the current keyboard keymap.", [&](auto &keymap_name) {
        auto kaymap_path = String::format("/Files/Keyboards/{}.kmap", keymap_name);
        return loadkey_set_keymap(keyboard_handle, kaymap_path);
    });

    return args.eval(argc, argv);
}

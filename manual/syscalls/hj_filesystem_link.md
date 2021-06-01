# hj_filesystem_link

```c++
HjResult hj_filesystem_link(
    const char *old_path,
    size_t old_path_size,
    const char *new_path,
    size_t new_path_size)
```

## Description

Create an hard-link at `new_path` to an existing file pointed by `old_path`.

## Parameters

- `old_path`: path to the file being linked to (const char*).
- `old_size`: size of the old path (size_t).
- `new_path`: path of the new link (const char*).
- `new_size`: size if the new path (size_t).

## Return

- SUCCESS: If the link has been created.
- ERR_BAD_ADDRESS
- ERR_NO_SUCH_FILE_OR_DIRECTORY: If `old_path` is non existing, or the parent of `new_path` is non existing.
- ERR_IS_A_DIRECTORY: If the `old_path` is a directory.
- ERR_NOT_A_DIRECTORY: If the parent of `new_path` is a directory.

Additional error code can be returned depending of the underlying filesystem.

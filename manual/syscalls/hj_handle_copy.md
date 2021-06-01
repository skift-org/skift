# hj_handle_copy

```c
HjResult hj_handle_copy(int source, int destination);
```

## Description

`hj_handle_copy` create a copy of an handle and store it in `destination`.
The `destination` and `source` will refer to the same underlying ressource.
If `destination` is a valid file descriptor then it close it first.

## Parameters

- `source`: Handle to be copy (int).
- `destination`: New handle (int).

## Return

- SUCCESS: If `source` was copied successfully.
- ERR_BAD_HANDLE: If `source` is not a valid.
- ERR_TOO_MANY_HANDLE: If there is too many handle in the current process.

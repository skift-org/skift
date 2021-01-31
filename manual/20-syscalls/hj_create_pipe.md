# hj_create_pipe

```c
Result hj_create_pipe(int *reader_handle, int *writer_handle);
```

## Description

`hj_create_pipe` create a pipe, which is a FIFO buffer used by two process for unidirectional data transfer.

## Parameters

- `reader_handle`: Handle to the reading end of the pipe (int*).
- `writer_handle`: Handle to the writing end of the pipe (int*).

## Return

- SUCCESS: If the pipe has been created.
- ERR_BAD_ADDRESS
- ERR_TOO_MANY_HANDLE

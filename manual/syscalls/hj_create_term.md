# hj_create_term

```c
HjResult hj_create_term(int *server_handle, int *client_handle);
```

## Description

`hj_create_term` create a terminal, which is a combinaison of two FIFO buffers used by one or more processes for bidirectional text transfer with a terminal application.

## Parameters

- `server_handle`: Handle to the server end of the term (int*).
- `client_handle`: Handle to the client end of the term (int*).

## Return

- SUCCESS: If the term was created.
- ERR_BAD_ADDRESS
- ERR_TOO_MANY_HANDLE: If there is too many handle in the current process.

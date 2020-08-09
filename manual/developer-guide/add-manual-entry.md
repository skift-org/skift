# Add a manual entry

Manual entries are put in /System/Manuals. To create one make a file called `[ENTRY_NAME].json`, replace `[ENTRY_NAME]` with the name of application. Add the contents:

```json
{
    "description": "put the description of the application here",
    "usage": "usage data here. eg. command [PARAMETER]",
    "example": "an example on how to use use it. eg command hello: olleh. The output of the command is not required"
}
```

# Skift config (draft)

## Name

.skconf - The main skiftOS config file format.

## Description

skconf is a keyvalue pair configuration file format inspire by windows's ini files.

```ini
# This is a comment

[section0]
key0: "string value"
key1: 1234
key2: true

[section1]
key0: [ "this", "is", "a", "array" ]
key1: [ 1, 2, 3, 4, 5, 6, 7 ]
```

### Data types
 - int
 - string
 - boolean
 - arrays

## Exemples

```c
#include <skift/config.h>

// Load the config file in memory.
config_t* config = config_load("exemple.skconf");

// Set section1's key0 to a int 12345.
config_seti("section1.key0", 12345);

if (config_has("section1.key0"))
{
// do somethings
int value = config_geti("section1.key0", 12345);
}

// Save the config file back to the disk.
config_save(config);

// Free the memory.
config_delete(config);
```

## See also

framework(3)
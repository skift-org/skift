#include <stdint.h>
#include <string.h>

int cstrEq(char const *str1, char const *str2) {
    while (*str1 && *str2) {
        if (*str1++ != *str2++)
            return 0;
    }
    return *str1 == *str2;
}

static char const *_names[] = {
#define ICON(id, name, code) name,
#include "icons.inc"
#undef ICON
};

static uint32_t _codepoints[] = {
#define ICON(id, name, code) code,
#include "icons.inc"
#undef ICON
};

uint32_t *_Mdi__codepointsStart() {
    return _codepoints;
}

uint32_t *_Mdi__codepointsEnd() {
    return _codepoints + sizeof(_codepoints) / sizeof(_codepoints[0]);
}

uint32_t _Mdi__byName(char const *query, size_t) {
    for (size_t i = 0; i < sizeof(_names) / sizeof(_names[0]); ++i) {
        if (cstrEq(_names[i], query))
            return _codepoints[i];
    }
    return 0;
}

char const *_Mdi__name(uint32_t query) {
    for (size_t i = 0; i < sizeof(_codepoints) / sizeof(_codepoints[0]); ++i) {
        if (_codepoints[i] == query)
            return _names[i];
    }
    return 0;
}

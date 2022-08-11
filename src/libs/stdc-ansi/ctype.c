#include <ctype.h>
#include <stddef.h>

// using https://koor.fr/C/cctype/Index.wp for ascii code

int isalnum(int c) {
    if (c <= 'z' && c >= 'a') {
        return 1;
    }
    if (c <= 'Z' && c >= 'A') {
        return 1;
    }
    if (c <= '9' && c >= '0') {
        return 1;
    }
    return 0;
}

int isalpha(int c) {
    if (c <= 'z' && c >= 'a') {
        return 1;
    }
    if (c <= 'Z' && c >= 'A') {
        return 1;
    }
    return 0;
}

int isblank(int c) {
    if (c == ' ' || c == '\t') {
        return 1;
    }
    return 0;
}

int iscntrl(int c) {
    if (c < 0x1f || c == 0x7f) {
        return 1;
    }
    return 0;
}

int isdigit(int c) {
    if (c <= '9' && c >= '0') {
        return 1;
    }
    return 0;
}

int isgraph(int c) {
    if (c >= 0x21 && c <= 0x7f) {
        return 1;
    }
    return 0;
}

int islower(int c) {

    if (c >= 'a' && c <= 'z') {
        return 1;
    }
    return 0;
}

int isprint(int c) {
    if (c >= 0x20 && c <= 0x7f) {
        return 1;
    }
    return 0;
}

int ispunct(int c) {
    char const punctuation[] = "!\"#$%&'()*+,-./:;<=>?@ [\\]^_`{|}~";

    for (size_t i = 0; i < sizeof(punctuation); i++) {
        if (punctuation[i] == c) {
            return 1;
        }
    }

    return 0;
}

int isspace(int c) {
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v') {
        return 1;
    } else {
        return 0;
    }
}

int isupper(int c) {
    if (c >= 'A' && c <= 'Z') {
        return 1;
    }
    return 0;
}

int isxdigit(int c) {
    if (isdigit(c)) {
        return 1;
    }
    if (c <= 'F' && c >= 'A') {
        return 1;
    }
    if (c <= 'f' && c >= 'a') {
        return 1;
    }
    return 0;
}

int tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        c -= 'A';
        c += 'a';
    }

    return c;
}

int toupper(int c) {
    if (c >= 'a' && c <= 'z') {
        c -= 'a';
        c += 'A';
    }
    return c;
}

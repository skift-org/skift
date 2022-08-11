#include <stdc-base/prelude.h>
#include <string.h>

/* --- 7.24.2 - Copying functions ------------------------------------------- */

void *memcpy(void *STDC_RESTRICT s1, void const *STDC_RESTRICT s2, size_t n) {
    char *STDC_RESTRICT p1 = (char *STDC_RESTRICT)s1;
    char const *STDC_RESTRICT p2 = (char const *STDC_RESTRICT)s2;
    while (n--)
        *p1++ = *p2++;
    return s1;
}

void *memmove(void *s1, void const *s2, size_t n) {
    char const *usrc = (char const *)s2;
    char *udest = (char *)s1;

    if (udest < usrc) {
        for (size_t i = 0; i < n; i++) {
            udest[i] = usrc[i];
        }
    } else if (udest > usrc) {
        for (size_t i = n; i > 0; i--) {
            udest[i - 1] = usrc[i - 1];
        }
    }

    return s1;
}

char *strcpy(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2) {
    size_t i = 0;

    while (s2[i] != 0) {
        s1[i] = s2[i];
        i++;
    }

    return s1;
}

char *strncpy(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2, size_t n) {
    size_t i = 0;

    while (s2[i] != 0 && i < n) {
        s1[i] = s2[i];
        i++;
    }

    return s1;
}

/* --- 7.24.3 - Concatenation functions ------------------------------------- */

char *strcat(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2) {
    size_t l2 = strlen(s2);
    s1 = s1 + strlen(s1); // append to the last

    for (size_t i = 0; i < l2; i++) {
        s1[i] = s2[i];
    }

    s1[l2] = 0;

    return s1;
}

char *strncat(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2, size_t n) {
    size_t l2 = strlen(s2);
    size_t i = 0;
    s1 = s1 + strlen(s1);

    for (i = 0; i < l2 && i < n; i++) {
        s1[i] = s2[i];
    }

    s1[i] = 0;

    return s1;
}

/* --- 7.24.4 - Comparison functions ---------------------------------------- */

int memcmp(void const *s1, void const *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        int diff = *(char *)s1 - *(char *)s2;
        if (diff != 0) {
            return diff;
        }
    }

    return 0;
}

int strcmp(char const *s1, char const *s2) {
    while (*s1 == *s2 && *s1) {
        s1++;
        s2++;
    };

    return (int)*s1 - (int)*s2;
}

// int strcoll(char const *s1, char const*s2) {}

int strncmp(char const *s1, char const *s2, size_t n) {
    while (*s1 == *s2 && *s1 && n) {
        s1++;
        s2++;
        n--;
    }

    if (n == 0) {
        return 0;
    }

    return (int)*s1 - (int)*s2;
}

// size_t strxfrm(char *STDC_RESTRICT s1, char const *STDC_RESTRICT s2, size_t n) {}

/* --- 7.24.5 - Search functions -------------------------------------------- */

// void *memchr(void const *s, int c, size_t n) {}

char *strchr(char const *s, int c) {
    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++) {
        if (s[i] == c) {
            return (char *)s + i;
        }
    }

    return NULL;
}

// size_t strcspn(char const *s1,  char const *s2) {}

// char *strpbrk(char const *s1,  char const *s2) {}

char *strrchr(char const *s, int c) {
    size_t len = strlen(s);
    size_t pos = len;

    while (s[pos] != c && pos-- != 0)
        ;

    if (pos == len) {
        return NULL;
    }

    return (char *)s + pos;
}

// size_t strspn(char const *s1,  char const *s2) {}

// char *strstr(char const *s1,  char const *s2) {}

// char *strtok(char *STDC_RESTRICT s1,  char const *STDC_RESTRICT s2) {}

/* --- 7.24.6 - Miscellaneous functions ------------------------------------- */

void *memset(void *s, int c, size_t n) {
    char *p = (char *)s;
    while (n--) {
        *p++ = c;
    }
    return s;
}

// char *strerror(int errnum) { }

size_t strlen(char const *s) {
    size_t i = 0;

    while (s[i++] != '\0')
        ;

    return i;
}

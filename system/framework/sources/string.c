#include <skift/generic.h>

#include "string.h"
#include "math.h"

void *memchr(const void *str, int c, size_t n)
{
    unsigned char *s = (unsigned char *)str;

    for (size_t i = 0; i < n; i++)
    {
        if (*(s + i) == c)
        {
            return (s + i);
        }
    }

    return NULL;
}

int memcmp(const void *str1, const void *str2, size_t n)
{
    const unsigned char *s1 = str1;
    const unsigned char *s2 = str2;

    for (size_t i = 0; i < n; i++)
    {
        if (*(s1 + i) != *(s2 + i))
        {
            return *(s1 + i) - *(s2 + i);
        }
    }

    return 0;
}

void *memmove(void *dest, const void *src, size_t n)
{
    size_t i;
    const unsigned char *usrc = src;
    unsigned char *udest = dest;

    if (udest < usrc)
    {
        for (i = 0; i < n; i++)
            udest[i] = usrc[i];
    }
    else if (udest > usrc)
    {
        for (i = n; i > 0; i--)
            udest[i - 1] = usrc[i - 1];
    }

    return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;

    for (size_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }

    return dest;
}

void *memset(void *str, int c, size_t n)
{
    char *s = str;

    for (size_t i = 0; i < n; i++)
    {
        *(s + i) = c;
    }

    return str;
}

void *memshift(char *mem, int shift, size_t n)
{
    void *dest = mem + shift;
    memmove(dest, mem, n);
    return dest;
}

char *strcat(char *dest, const char *src)
{
    unsigned int dest_size = strlen(dest);
    unsigned int src_size = strlen(src);

    char *d = dest;

    for (size_t i = 0; i < src_size; i++)
    {
        d[dest_size + i] = src[i];
    }

    d[dest_size + src_size] = '\0';

    return dest;
}

char *strncat(char *s1, const char *s2, size_t n)
{
    char *s = s1;
    /* Find the end of S1.  */
    s1 += strlen(s1);
    size_t ss = strnlen(s2, n);
    s1[ss] = '\0';
    memcpy(s1, s2, ss);
    return s;
}

char *strchr(const char *p, int ch)
{
    char c;

    c = ch;
    for (;; ++p)
    {
        if (*p == c)
            return ((char *)p);
        if (*p == '\0')
            return (NULL);
    }
}

int strcmp(const char *stra, const char *strb)
{
    unsigned int i;

    for (i = 0; stra[i] == strb[i]; i++)
    {
        if (stra[i] == '\0')
            return 0;
    }

    return stra[i] - strb[i];
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    register unsigned char u1, u2;

    while (n-- > 0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2)
            return u1 - u2;

        if (u1 == '\0')
            return 0;
    }
    return 0;
}

int strcoll(const char *s1, const char *s2)
{
    char t1[1 + strxfrm(0, s1, 0)];
    strxfrm(t1, s1, sizeof(t1));
    char t2[1 + strxfrm(0, s2, 0)];
    strxfrm(t2, s2, sizeof(t2));
    return strcmp(t1, t2);
}

char *strcpy(char *s1, const char *s2)
{
    char *s = s1;
    while ((*s++ = *s2++) != 0)
        ;
    return (s1);
}

char *strncpy(char *s1, const char *s2, size_t n)
{
    size_t size = strnlen(s2, n);
    if (size != n)
        memset(s1 + size, '\0', n - size);
    return memcpy(s1, s2, size);
}

size_t strcspn(const char *string, const char *chars)
{
    register char c, *p, *s;

    for (s = (char *)string, c = *s; c != 0; s++, c = *s)
    {
        for (p = (char *)chars; *p != 0; p++)
        {
            if (c == *p)
            {
                return s - string;
            }
        }
    }
    return s - string;
}

char *strerror(int errnum)
{
    (void)(errnum);
    return "Error";
}

size_t strlen(const char *str)
{
    size_t lenght = 0;
    while (str[lenght])
        lenght++;
    return lenght;
}

size_t strnlen(const char *s, size_t maxlen)
{
    size_t len;

    for (len = 0; len < maxlen; len++, s++)
    {
        if (!*s)
            break;
    }
    return (len);
}

char *strpbrk(const char *s, const char *accept)
{
    s += strcspn(s, accept);
    return *s ? (char *)s : NULL;
}

char *strrchr(const char *s, int c)
{

    char *rtnval = 0;

    do
    {
        if (*s == c)
            rtnval = (char *)s;
    } while (*s++);
    return (rtnval);
}

size_t strspn(const char *p, const char *s)
{
    int i, j;

    for (i = 0; p[i]; i++)
    {
        for (j = 0; s[j]; j++)
        {
            if (s[j] == p[i])
                break;
        }
        if (!s[j])
            break;
    }
    return (i);
}

char *strstr(const char *s1, const char *s2)
{
    const char *p = s1;
    const size_t len = strlen(s2);

    for (; (p = strchr(p, *s2)) != 0; p++)
    {
        if (strncmp(p, s2, len) == 0)
            return (char *)p;
    }
    return NULL;
}

char *strtok(char *s, char** state, const char *delim)
{
    register int ch;

    if (s == 0)
        s = *state;
    do
    {
        if ((ch = *s++) == '\0')
            return 0;
    } while (strchr(delim, ch));
    --s;
    *state = s + strcspn(s, delim);
    if (**state != 0)
        **state++ = 0;
    return s;
}

size_t strxfrm(char *dest, const char *src, size_t n)
{
    size_t len;

    len = strlen(src);
    (void)memcpy((void *)dest, (void *)src, min(n, len + 1));

    return len;
}

void strapd(char * str, char c)
{
    unsigned int len = strlen(str);
    str[len] = c;
    str[len + 1] = '\0';
}

void strbs(char *str)
{
    unsigned int len = strlen(str);
    str[len - 1] = '\0';
}

void strnapd(char * str, char c, size_t n)
{
    if (n > 1)
    {
        unsigned int len = strlen(str);

        if (len <= n - 2)
        {
            str[len] = c;
            str[len + 1] = '\0';
        }
    }
}

void strrvs(char * str)
{
    int c, i, j;
    for (i = 0, j = strlen(str) - 1; i < j; i++, j--)
    {
        c = str[i];
        str[i] = str[j];
        str[j] = c;
    }
}

void strleadtrim(char * str, char c)
{
    char *start = str;
    
    while (*start == c)
        start++;

    memmove(str, start, strlen(str + 1));
}

void strtrailtrim(char * str, char c)
{
    char *end = str + strlen(str) - 1;

    while (end > str && *end == c)
        end--;

    end[1] = '\0';
}
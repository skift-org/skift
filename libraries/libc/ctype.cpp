
#include <ctype.h>

int isprint(int c)
{
    if (c >= 0x20 && c <= 0x7e)
    {
        return 1;
    }

    return 0;
}

int islower(int c)
{
    if (c >= 'a' && c <= 'z')
    {
        return 1;
    }

    return 0;
}

int isupper(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return 1;
    }

    return 0;
}

int isalpha(int c)
{
    if (islower(c) || isupper(c))
    {
        return 1;
    }

    return 0;
}

int isdigit(int c)
{
    if (c >= '0' && c <= '9')
    {
        return 1;
    }

    return 0;
}

int toupper(int c)
{
    if (c >= 'a' && c <= 'z')
    {
        return c - 'a' + 'A';
    }
    return c;
}

int tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - 'A' + 'a';
    }
    return c;
}

int isspace(int c)
{
    return (c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == ' ');
}

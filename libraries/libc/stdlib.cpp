#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <abi/Syscalls.h>

void abort()
{
    hj_process_exit(PROCESS_FAILURE);
    ASSERT_NOT_REACHED();
}

int atoi(const char *str)
{
    int n = 0, neg = 0;
    while (isspace(*str))
        str++;
    switch (*str)
    {
    case '-':
        neg = 1;
        break;
    case '+':
        str++;
        break;
    }
    /* Compute n as a negative number to avoid overflow on INT_MIN */
    while (isdigit(*str))
        n = 10 * n - (*str++ - '0');
    return neg ? n : -n;
}

long int strtol(const char *str, char **end, int base)
{
    // skip leading space
    while (*str)
    {
        if (*str == '+')
            str++;
        if (!isspace(*str))
            break;
        str++;
    }

    bool negative = false;
    if (*str == '-')
    {
        negative = true;
        str++;
    }

    unsigned long number = strtoul(str, end, base);
    if (negative)
        return static_cast<long>((~number) + 1);
    return number;
}

long long int strtoll(const char *str, char **end, int base)
{
    // TODO: implement this properly
    return (long long)strtol(str, end, base);
}

char *getenv(const char *name)
{
    __unused(name);
    // Pretend the environment variable doesn't exist
    return NULL;
}

void exit(int status)
{
    hj_process_exit(status);
    __builtin_unreachable();
}

int system(const char *command)
{
    __unused(command);
    // TODO: implement this
    return -1;
}

#ifndef __KERNEL__

double strtod(const char *nptr, char **endptr)
{
    int sign = 1;
    if (*nptr == '-')
    {
        sign = -1;
        nptr++;
    }

    long long decimal_part = 0;

    while (*nptr && *nptr != '.')
    {
        if (*nptr < '0' || *nptr > '9')
        {
            break;
        }
        decimal_part *= 10LL;
        decimal_part += (long long)(*nptr - '0');
        nptr++;
    }

    double sub_part = 0;
    double multiplier = 0.1;

    if (*nptr == '.')
    {
        nptr++;

        while (*nptr)
        {
            if (*nptr < '0' || *nptr > '9')
            {
                break;
            }

            sub_part += multiplier * (*nptr - '0');
            multiplier *= 0.1;
            nptr++;
        }
    }

    double expn = (double)sign;

    if (*nptr == 'e' || *nptr == 'E')
    {
        nptr++;

        int exponent_sign = 1;

        if (*nptr == '+')
        {
            nptr++;
        }
        else if (*nptr == '-')
        {
            exponent_sign = -1;
            nptr++;
        }

        int exponent = 0;

        while (*nptr)
        {
            if (*nptr < '0' || *nptr > '9')
            {
                break;
            }
            exponent *= 10;
            exponent += (*nptr - '0');
            nptr++;
        }

        expn = pow(10.0, (double)(exponent * exponent_sign));
    }

    if (endptr)
    {
        *endptr = (char *)nptr;
    }
    double result = ((double)decimal_part + sub_part) * expn;
    return result;
}

float strtof(const char *nptr, char **endptr)
{
    return strtod(nptr, endptr);
}

double atof(const char *nptr)
{
    return strtod(nptr, nullptr);
}

#endif

void *bsearch(
    const void *key,
    const void *base,
    size_t count,
    size_t size,
    int (*compare)(const void *, const void *))
{
    // Invariant: Element is in the interval [i, j).
    size_t i = 0;
    size_t j = count;

    while (i < j)
    {
        size_t k = (j - i) / 2;
        const auto *element = reinterpret_cast<const char *>(base) + (i + k) * size;
        auto res = compare(key, element);
        if (res < 0)
        {
            j = i + k;
        }
        else if (res > 0)
        {
            i = i + k + 1;
        }
        else
        {
            return const_cast<char *>(element);
        }
    }
    assert(i == j);

    return nullptr;
}

void qsort(void *base, size_t count, size_t size,
           int (*compare)(const void *, const void *))
{
    // TODO: implement a faster sort
    for (size_t i = 0; i < count; i++)
    {
        void *u = (void *)((uintptr_t)base + i * size);
        for (size_t j = i + 1; j < count; j++)
        {
            void *v = (void *)((uintptr_t)base + j * size);
            if (compare(u, v) <= 0)
            {
                continue;
            }

            // swap u and v
            char *u_bytes = (char *)u;
            char *v_bytes = (char *)v;
            for (size_t k = 0; k < size; k++)
            {
                char temp = u_bytes[k];
                u_bytes[k] = v_bytes[k];
                v_bytes[k] = temp;
            }
        }
    }
}

// this function is copied from newlib and available under a BSD license
unsigned long strtoul(const char *__restrict nptr, char **__restrict endptr, int base)
{
    const unsigned char *s = (const unsigned char *)nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0, any, cutlim;

    do
    {
        c = *s++;
    } while (isspace(c));
    if (c == '-')
    {
        neg = 1;
        c = *s++;
    }
    else if (c == '+')
    {
        c = *s++;
    }
    if ((base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X'))
    {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
    {
        base = c == '0' ? 8 : 10;
    }
    cutoff = ULONG_MAX / (unsigned long)base;
    cutlim = ULONG_MAX % (unsigned long)base;
    for (acc = 0, any = 0;; c = *s++)
    {
        if (isdigit(c))
        {
            c -= '0';
        }
        else if (isalpha(c))
        {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        }
        else
        {
            break;
        }
        if (c >= base)
        {
            break;
        }
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
        {
            any = -1;
        }
        else
        {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0)
    {
        acc = ULONG_MAX;
    }
    else if (neg)
    {
        acc = -acc;
    }
    if (endptr != 0)
    {
        *endptr = (char *)(any ? (char *)s - 1 : nptr);
    }
    return (acc);
}
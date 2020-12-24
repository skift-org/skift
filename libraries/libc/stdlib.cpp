#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <libsystem/Logger.h>
#include <libsystem/process/Process.h>

void __no_return exit(int status)
{
    process_exit(status);
}

int system(const char *command)
{
    int pid = -1;
    auto result = process_run(command, &pid);

    if (result != SUCCESS)
    {
        return -1;
    }

    int exit_value = -1;
    result = process_wait(pid, &exit_value);

    if (result != SUCCESS)
    {
        return -1;
    }

    return exit_value;
}

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

int atoi(const char *s)
{
    int n = 0;
    int neg = 0;
    while (isspace(*s))
    {
        s++;
    }
    switch (*s)
    {
    case '-':
        neg = 1;
        /* Fallthrough */
    case '+':
        s++;
    }
    while (isdigit(*s))
    {
        n = 10 * n - (*s++ - '0');
    }
    /* The sign order may look incorrect here but this is correct as n is calculated
	 * as a negative number to avoid overflow on INT_MAX.
	 */
    return neg ? n : -n;
}

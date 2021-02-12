#include <assert.h>
#include <ctype.h>
#include <libutils/Scanner.h>
#include <stdio.h>
#include <string.h>

enum class ScanfType
{
    CHAR,
    SHORT,
    INTMAX,
    L,
    LL,
    PTRDIFF,
    SIZE_T,
    INT
};

static void store_int(void *dest, ScanfType size, unsigned long long i)
{
    switch (size)
    {
    case ScanfType::CHAR:
        *(char *)dest = i;
        break;
    case ScanfType::SHORT:
        *(short *)dest = i;
        break;
    case ScanfType::INTMAX:
        *(intmax_t *)dest = i;
        break;
    case ScanfType::L:
        *(long *)dest = i;
        break;
    case ScanfType::LL:
        *(long long *)dest = i;
        break;
    case ScanfType::PTRDIFF:
        *(ptrdiff_t *)dest = i;
        break;
    case ScanfType::SIZE_T:
        *(size_t *)dest = i;
        break;
    /* fallthrough */
    case ScanfType::INT:
    default:
        *(int *)dest = i;
        break;
    }
}

// Taken from mlibc. See https://github.com/managarm/mlibc/blob/684702ec25178671fde182319e932f6684c09f51/options/ansi/generic/stdio-stubs.cpp#L350
// Credits to original author Geert Custers
template <typename H>
static int do_scanf(H &handler, Scanner &scan, va_list args)
{
    int match_count = 0;
    while (scan.do_continue())
    {
        if (isspace(scan.current()))
        {
            while (isspace(scan.peek(1)))
                scan.foreward();
            while (isspace(handler.look_ahead()))
                handler.consume();
            continue;
        }

        if (scan.current() != '%' || scan.peek(1) == '%')
        {
            scan.skip('%');
            char c = handler.consume();
            if (c != scan.current())
                break;
            continue;
        }

        void *dest = NULL;
        /* %n$ format */
        if (isdigit(scan.current()) && scan.peek(1) == '$')
        {
            /* TODO: dest = get_arg_at_pos(args, *fmt -'0'); */
            scan.foreward(3);
        }
        else
        {
            dest = va_arg(args, void *);
            scan.foreward();
        }

        int width = 0;
        if (scan.skip('*'))
        {
            continue;
        }
        else if (scan.skip('\''))
        {
            /* TODO: numeric seperators locale stuff */
            fprintf(stdlog, "do_scanf: \' not implemented!\n");
            continue;
        }
        else if (scan.skip('m'))
        {
            /* TODO: allocate buffer for them */
            fprintf(stdlog, "do_scanf: m not implemented!\n");
            continue;
        }
        else if (isdigit(scan.current()))
        {
            /* read in width specifier */
            width = 0;
            while (isdigit(scan.current()))
            {
                width = width * 10 + (scan.current() - '0');
                scan.foreward();
                continue;
            }
        }

        /* type modifiers */
        ScanfType type = ScanfType::INT;
        unsigned int base = 10;
        switch (scan.current())
        {
        case 'h':
        {
            if (scan.peek(1) == 'h')
            {
                type = ScanfType::CHAR;
                scan.foreward(2);
                break;
            }
            type = ScanfType::SHORT;
            scan.foreward();
            break;
        }
        case 'j':
        {
            type = ScanfType::INTMAX;
            scan.foreward();
            break;
        }
        case 'l':
        {
            if (scan.peek(1) == 'l')
            {
                type = ScanfType::LL;
                scan.foreward(2);
                break;
            }
            type = ScanfType::L;
            scan.foreward();
            break;
        }
        case 'L':
        {
            type = ScanfType::LL;
            scan.foreward();
            break;
        }
        case 'q':
        {
            type = ScanfType::LL;
            scan.foreward();
            break;
        }
        case 't':
        {
            type = ScanfType::PTRDIFF;
            scan.foreward();
            break;
        }
        case 'z':
        {
            type = ScanfType::SIZE_T;
            scan.foreward();
            break;
        }
        case '0':
        {
            if (scan.peek(1) == 'x' || scan.peek(1) == 'X')
            {
                base = 16;
                scan.foreward((size_t)2u);
                break;
            }
            base = 8;
            scan.foreward();
            break;
        }
        }

        switch (scan.current())
        {
        case 'd':
        case 'u':
            base = 10;
            /* fallthrough */
        case 'i':
        {
            unsigned long long res = 0;
            char c = handler.look_ahead();
            switch (base)
            {
            case 10:
                while (c >= '0' && c <= '9')
                {
                    handler.consume();
                    res = res * 10 + (c - '0');
                    c = handler.look_ahead();
                }
                break;
            case 16:
                if (c == '0')
                {
                    handler.consume();
                    c = handler.look_ahead();
                    if (c == 'x')
                    {
                        handler.consume();
                        c = handler.look_ahead();
                    }
                }
                while (true)
                {
                    if (c >= '0' && c <= '9')
                    {
                        handler.consume();
                        res = res * 16 + (c - '0');
                    }
                    else if (c >= 'a' && c <= 'f')
                    {
                        handler.consume();
                        res = res * 16 + (c - 'a');
                    }
                    else if (c >= 'A' && c <= 'F')
                    {
                        handler.consume();
                        res = res * 16 + (c - 'A');
                    }
                    else
                    {
                        break;
                    }
                    c = handler.look_ahead();
                }
                break;
            case 8:
                while (c >= '0' && c <= '7')
                {
                    handler.consume();
                    res = res * 10 + (c - '0');
                    c = handler.look_ahead();
                }
                break;
            }
            if (dest)
                store_int(dest, type, res);
            break;
        }
        case 'o':
        {
            unsigned long long res = 0;
            char c = handler.look_ahead();
            while (c >= '0' && c <= '7')
            {
                handler.consume();
                res = res * 10 + (c - '0');
                c = handler.look_ahead();
            }
            if (dest)
                store_int(dest, type, res);
            break;
        }
        case 'x':
        case 'X':
        {
            unsigned long long res = 0;
            char c = handler.look_ahead();
            if (c == '0')
            {
                handler.consume();
                c = handler.look_ahead();
                if (c == 'x')
                {
                    handler.consume();
                    c = handler.look_ahead();
                }
            }
            while (true)
            {
                if (c >= '0' && c <= '9')
                {
                    handler.consume();
                    res = res * 16 + (c - '0');
                }
                else if (c >= 'a' && c <= 'f')
                {
                    handler.consume();
                    res = res * 16 + (c - 'a');
                }
                else if (c >= 'A' && c <= 'F')
                {
                    handler.consume();
                    res = res * 16 + (c - 'A');
                }
                else
                {
                    break;
                }
                c = handler.look_ahead();
            }
            if (dest)
                store_int(dest, type, res);
            break;
        }
        case 's':
        {
            char *typed_dest = (char *)dest;
            char c = handler.look_ahead();
            int count = 0;
            while (c && !isspace(c))
            {
                handler.consume();
                if (typed_dest)
                    typed_dest[count] = c;
                c = handler.look_ahead();
                count++;
                if (width && count >= width)
                    break;
            }
            if (typed_dest)
                typed_dest[count + 1] = '\0';
            break;
        }
        case 'c':
        {
            char *typed_dest = (char *)dest;
            char c = handler.look_ahead();
            int count = 0;
            if (!width)
                width = 1;
            while (c && count < width)
            {
                handler.consume();
                if (typed_dest)
                    typed_dest[count] = c;
                c = handler.look_ahead();
                count++;
            }
            break;
        }
        case '[':
        {
            scan.foreward();
            int invert = 0;
            if (scan.current() == '^')
            {
                invert = 1;
                scan.foreward();
            }

            char scanset[257];
            memset(&scanset[0], invert, sizeof(char) * 257);
            scanset[0] = '\0';

            if (scan.current() == '-')
            {
                scan.foreward();
                scanset[1 + '-'] = 1 - invert;
            }
            else if (scan.current() == ']')
            {
                scan.foreward();
                scanset[1 + ']'] = 1 - invert;
            }

            for (; scan.current() != ']'; scan.foreward())
            {
                if (scan.current() == 0)
                    return EOF;
                if (scan.current() == '-' && scan.current() != ']')
                {
                    scan.foreward();
                    for (char c = scan.peek(-2); c < scan.current(); c++)
                        scanset[1 + c] = 1 - invert;
                }
                scanset[1 + scan.current()] = 1 - invert;
            }

            char *typed_dest = (char *)dest;
            int count = 0;
            char c = handler.look_ahead();
            while (c && count < width)
            {
                handler.consume();
                if (!scanset[1 + c])
                    break;
                if (typed_dest)
                    typed_dest[count] = c;
                c = handler.look_ahead();
                count++;
            }
            if (typed_dest)
                typed_dest[count] = '\0';
            break;
        }
        case 'p':
        {
            unsigned long long res = 0;
            char c = handler.look_ahead();
            if (c == '0')
            {
                handler.consume();
                c = handler.look_ahead();
                if (c == 'x')
                {
                    handler.consume();
                    c = handler.look_ahead();
                }
            }
            while (true)
            {
                if (c >= '0' && c <= '9')
                {
                    handler.consume();
                    res = res * 16 + (c - '0');
                }
                else if (c >= 'a' && c <= 'f')
                {
                    handler.consume();
                    res = res * 16 + (c - 'a');
                }
                else if (c >= 'A' && c <= 'F')
                {
                    handler.consume();
                    res = res * 16 + (c - 'A');
                }
                else
                {
                    break;
                }
                c = handler.look_ahead();
            }
            void **typed_dest = (void **)dest;
            *typed_dest = (void *)(uintptr_t)res;
            break;
        }
        case 'n':
        {
            int *typed_dest = (int *)dest;
            if (typed_dest)
                *typed_dest = handler.num_consumed;
            continue;
        }
        }
        if (dest)
            match_count++;
    }
    return match_count;
}

int fscanf(FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfscanf(stream, format, args);
    va_end(args);
    return result;
}

int vfscanf(FILE *file, const char *format, va_list args)
{
    struct
    {
        char look_ahead()
        {
            char c;
            size_t actual_size = fread(&c, 1, 1, file);
            if (actual_size)
                fseek(file, 1, WHENCE_HERE);
            return actual_size ? c : 0;
        }

        char consume()
        {
            char c;
            size_t actual_size = fread(&c, 1, 1, file);
            if (actual_size)
                num_consumed++;
            return actual_size ? c : 0;
        }

        FILE *file;
        int num_consumed = 0;
    } handler = {file};

    StringScanner scan{format, strlen(format)};
    return do_scanf(handler, scan, args);
}

int sscanf(const char *str, const char *format, ...)
{
    class
    {
    public:
        char look_ahead()
        {
            return *buffer;
        }

        char consume()
        {
            num_consumed++;
            return *buffer++;
        }

        const char *buffer;
        int num_consumed = 0;
    } handler = {str};
    va_list args;
    va_start(args, format);
    StringScanner scan{format, strlen(format)};
    int result = do_scanf(handler, scan, args);
    va_end(args);
    return result;
}

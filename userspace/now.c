#include <libsystem/iostream.h>
#include <libsystem/time.h>

int main(int argc, const char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    datetime_t datetime = datetime_now();
    timestamp_t timestamp = timestamp_now();

    printf("%d-%d-%d %d/%d/%d (%dsec since 1970)\n",datetime.hour, datetime.minute, datetime.second, datetime.day, datetime.month, datetime.year, timestamp);

    return 0;
}

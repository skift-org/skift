#include <skift/io.h>
#include <skift/process.h>

int main(int argc, const char **argv)
{
    while(1) /* keep busy */;

    while (1)
        sk_io_print("Hello from userspace!\n");

    return 0;
}